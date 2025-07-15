using System;
using System.Diagnostics;
using System.IO.Ports;
using System.Net.Sockets;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Security.AccessControl;
using System.Text;
using System.Threading;


namespace HelloWorld
{
    class Program
    {
        static int bellState = 0;
        static int previousBellInput = 0;


        // controllerID variable list, these change from locomotive to locomotive
        // you can find your specific locomotive's controllerID variables with
        // string controlListRaw = Marshal.PtrToStringAnsi(GetControllerList());
        // then print that to the console, copy it, and count up (first control is ID 0)
        static int throttleID = 27;
        const int virtualThrottleID = 28;
        static int reverserID = 26;
        static int trainBrakeID = 30;
        const int virtualTrainBrakeID = 31;
        static int locoBrakeID = 29;
        static int hornID = 22;
        static int bellID = 43;

        // defining the dll functions
        // if the dll cannot be found, ensure the path is right (if you used the default directory you'll be fine)
        [DllImport("C:\\Program Files (x86)\\Steam\\steamapps\\common\\railworks\\plugins\\RailDriver64.dll")]
        public static extern IntPtr GetLocoName();

        [DllImport("C:\\Program Files (x86)\\Steam\\steamapps\\common\\railworks\\plugins\\RailDriver64.dll")]
        static extern void SetRailDriverConnected(bool isConnected);

        [DllImport("C:\\Program Files (x86)\\Steam\\steamapps\\common\\railworks\\plugins\\RailDriver64.dll")]
        public static extern IntPtr GetControllerList();

        [DllImport("C:\\Program Files (x86)\\Steam\\steamapps\\common\\railworks\\plugins\\RailDriver64.dll")]
        internal static extern Single GetCurrentControllerValue(int Control);

        [DllImport("C:\\Program Files (x86)\\Steam\\steamapps\\common\\railworks\\plugins\\RailDriver64.dll")]
        internal static extern void SetControllerValue(int Control, float Value);
        
        static void Main(string[] args)
        {
            using TcpClient client = new TcpClient("localhost", 5555);
            using StreamReader reader = new StreamReader(client.GetStream());
            SetRailDriverConnected(true);
            //Console.WriteLine("Connected to Python serial forwarder.");

            // test if this is still needed to read some of the functions from the dll
            byte[] buffer = new byte[64];
            Marshal.Copy(GetLocoName(), buffer, 0, buffer.Length);
            Console.WriteLine(BitConverter.ToString(buffer));

            // this might still work
            float value = GetCurrentControllerValue(hornID);
            Console.WriteLine($"Control ID 20 value: {value}");

            // keeping this here for syntax
            string controlListRaw = Marshal.PtrToStringAnsi(GetControllerList());
            Console.WriteLine(controlListRaw);

            // this is definately needed (sends the incoming data to be parsed)
            while (true)
            {
                string line = reader.ReadLine();
                if (line != null)
                {
                    //Console.WriteLine(">> " + line);
                    ParseLine(line); // ← now in the same class
                }
            }
        }

        static void ParseLine(string line)
        {
            SetRailDriverConnected(true); // don't ever touch this

            var parsedData = new Dictionary<string, int>();
            string[] sections = line.Split(',');

            foreach (var section in sections)
            {
                var parts = section.Split(':');
                if (parts.Length == 2 && int.TryParse(parts[1], out int value))
                {
                    parsedData[parts[0]] = value;
                }
            }
            
            //parcing the data from the python script into variables
            int HR = parsedData.GetValueOrDefault("HR", 0);
            int BL = parsedData.GetValueOrDefault("BL", 0);
            int SD = parsedData.GetValueOrDefault("SD", 0);
            int AB = parsedData.GetValueOrDefault("AB", 0);
            int IB = parsedData.GetValueOrDefault("IB", 0);
            int IL = parsedData.GetValueOrDefault("IL", 0);
            int RS = parsedData.GetValueOrDefault("RS", 0);
            int TH = parsedData.GetValueOrDefault("TH", 0);
            int DB = parsedData.GetValueOrDefault("DB", 0);

            // for input debugging
            /*
            Console.WriteLine($"horn state: {HR}");
            Console.WriteLine($"bell state: {BL}");
            Console.WriteLine($"sand state: {SD}");
            Console.WriteLine($"automatic brake state: {AB}");
            Console.WriteLine($"independent brake state: {IB}");
            Console.WriteLine($"reverser state: {RS}");
            Console.WriteLine($"throttle state: {TH}");
            Console.WriteLine($"dynamic brake state: {DB}");
            */
            

            // sending combined throttle / brake position
            float normalizedThrottle = GetNormalizedThrottle(TH, DB);
            SetControllerValue(throttleID, normalizedThrottle);
            SetControllerValue(virtualThrottleID, normalizedThrottle);

            // sending horn state
            SetControllerValue(hornID, HR);
            float currentValuew = GetControllerList();

            // sending the automatic brake position
            float autoBrake = MapAutoBrakeValue(AB);
            SetControllerValue(trainBrakeID, autoBrake);
            SetControllerValue(virtualTrainBrakeID, autoBrake);

            // sending the independent brake position
            float mappedIB = MapIndependentBrake(IB);
            SetControllerValue(locoBrakeID, mappedIB);

            // sending the reverser position
            float mappedReverser = MappedReverser(RS);
            SetControllerValue(reverserID, mappedReverser);

            // sending the bell state
            int bellOutput = UpdateBellState(BL, HR);
            SetControllerValue(bellID, bellOutput);

            // control status monitoring
            Console.WriteLine($"Horn input {HR} / game output {GetCurrentControllerValue(hornID)}");
            Console.WriteLine($"Bell input {BL} / game output {GetCurrentControllerValue(bellID)}");
            Console.WriteLine($"TrainBrake serial input {AB} / game input {autoBrake} / game output {GetCurrentControllerValue(trainBrakeID)}");
            Console.WriteLine($"LocoBrake serial input {IB} / game input {mappedIB} / game output {GetCurrentControllerValue(locoBrakeID)}");
            Console.WriteLine($"Reverser serial input {RS} / game input {mappedReverser} / game output {GetCurrentControllerValue(reverserID)}");
            Console.WriteLine($"Throttle and Dynamic serial input {TH}, {DB} / game input {normalizedThrottle} / game output {GetCurrentControllerValue(throttleID)}");

        }
        static float GetNormalizedThrottle(int throttleNotch, int dynamicBrake)
        {
            // Dynamic brake overrides throttle if active
            if (dynamicBrake > 0)
            {
                dynamicBrake = Math.Clamp(dynamicBrake, 0, 255);

                float flipped = 1.0f - (dynamicBrake / 255f);
                return flipped * 0.5f;  // Scale to 0.0 – 0.5
            }

            throttleNotch = Math.Clamp(throttleNotch, 0, 8);
            return 0.5f + ((float)throttleNotch / 8f) * 0.5f;
        }
        static float MapAutoBrakeValue(int raw)
        {
            raw = Math.Clamp(raw, 0, 255);

            if (raw == 0)
                return 0.0f;
            if (raw == 1)
                return 0.1f;
            if (raw >= 2 && raw <= 252)
            {
                // Linearly scale 2–252 → 0.11–0.50
                float fraction = (raw - 2) / 250f; // 250 steps from 2 to 252
                return 0.11f + fraction * (0.50f - 0.11f);
            }
            if (raw == 253)
                return 0.60f;
            if (raw == 254)
                return 0.80f;
            return 1.0f; // raw == 255
        }
        static float MapIndependentBrake(int raw)
        {
            // simple on or off logic (surfliner CC)
            if (raw >= 128)
            {
                return 1;
            } else
            {
                return 0;
            }
        }

        static float MappedReverser(int raw)
        {
            raw = Math.Clamp(raw, 0, 255);  // ensuring data is within range

            // really shoulda been a switch statement tbf
            if (raw == 0)
            {
                return -1;
            } 
            else if (raw == 128)
            {
                return 0;
            }
            else if (raw == 255)
            {
                return 1;
            } else
            {
                return 0;
            }

            
        }
        static int UpdateBellState(int bellInput, int hornInput)
        {

            // Toggle on rising edge of bell input
            if (previousBellInput == 0 && bellInput == 1)
            {
                bellState = bellState == 0 ? 1 : 0;
            }

            previousBellInput = bellInput;

            // If horn is pressed, force bell on
            if (hornInput == 1)
            {
                bellState = 1;
            }

            return bellState;
        }
    }
}
