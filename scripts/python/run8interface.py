import serial
import socket

# Configure serial connection to Arduino
SERIAL_PORT = 'COM4'  # Change this to match your system (e.g., /dev/ttyUSB0 on Linux)
BAUD_RATE = 9600

# Configure UDP connection to Run8
RUN8_IP = '127.0.0.1'
RUN8_PORT = 9696  # Set in Run8's controls menu

# Create UDP socket
udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
parsed_data = {}
line = ""

try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    print(f"Listening on {SERIAL_PORT} at {BAUD_RATE} baud...")
    
    while True:
       line = ser.readline().decode('utf-8').strip()

        if not line:  
            continue  # ignore empty linesd

        print("Received:", line)

        try:
            sections = line.split(',')
            for section in sections:
                key, value = section.split(':')
                parsed_data[key] = int(value)  # Convert value to integer
        except ValueError:
            print("Malformed line, skipping:", line)
            continue
            
        HR = parsed_data.get("HR", 0)
        BL = parsed_data.get("BL", 0)
        SD = parsed_data.get("SD", 0)
        AB = parsed_data.get("AB", 0)
        IB = parsed_data.get("IB", 0)
        IL = parsed_data.get("IL", 0)
        RS = parsed_data.get("RS", 0)
        TH = parsed_data.get("TH", 0)
        DB = parsed_data.get("DB", 0)
        #print(f"HR: {HR}, BL: {BL}, SD: {SD}")
        
        horn_data = [96, 0, 8]
        horn_data.append(HR)
        horn_crc = horn_data[0]
        for i in range(1, len(horn_data)):  
            horn_crc ^= horn_data[i]
        horn_data.append(horn_crc)
        horn_byte_data = bytes(horn_data)
        
        bell_data = [96, 0, 2]
        bell_data.append(BL)
        bell_crc = bell_data[0]
        for i in range(1, len(bell_data)):  
            bell_crc ^= bell_data[i]
        bell_data.append(bell_crc)
        bell_byte_data = bytes(bell_data)

        sand_data = [96, 0, 15]
        sand_data.append(SD)
        sand_crc = sand_data[0]
        for i in range(1, len(sand_data)):  
            sand_crc ^= sand_data[i]
        sand_data.append(sand_crc)
        sand_byte_data = bytes(sand_data)

        autoBrake_data = [96, 0, 18]
        autoBrake_data.append(AB)
        autoBrake_crc = autoBrake_data[0]
        for i in range(1, len(autoBrake_data)):  
            autoBrake_crc ^= autoBrake_data[i]
        autoBrake_data.append(autoBrake_crc)
        autoBrake_byte_data = bytes(autoBrake_data)

        indyBrake_data = [96, 0, 9]
        indyBrake_data.append(IB)
        indyBrake_crc = indyBrake_data[0]
        for i in range(1, len(indyBrake_data)): 
            indyBrake_crc ^= indyBrake_data[i]
        indyBrake_data.append(indyBrake_crc)
        indyBrake_byte_data = bytes(indyBrake_data)
        
        indyBail_data = [96, 0, 10]
        indyBail_data.append(IL)
        indyBail_crc = indyBail_data[0]
        for i in range(1, len(indyBail_data)):  
            indyBail_crc ^= indyBail_data[i]
        indyBail_data.append(indyBail_crc)
        indyBail_byte_data = bytes(indyBail_data)

        throttle_data = [96, 0, 16]
        throttle_data.append(TH)
        throttle_crc = throttle_data[0]
        for i in range(1, len(throttle_data)):  
            throttle_crc ^= throttle_data[i]
        throttle_data.append(throttle_crc)
        throttle_byte_data = bytes(throttle_data)

        reverser_data = [96, 0, 14]
        reverser_data.append(RS)
        reverser_crc = reverser_data[0]
        for i in range(1, len(reverser_data)):  
            reverser_crc ^= reverser_data[i]
        reverser_data.append(reverser_crc)
        reverser_byte_data = bytes(reverser_data)
        
        dynamic_data = [96, 0, 4]
        dynamic_data.append(DB)
        dynamic_crc = dynamic_data[0]
        for i in range(1, len(dynamic_data)):  
            dynamic_crc ^= dynamic_data[i]
        dynamic_data.append(dynamic_crc)
        dynamic_byte_data = bytes(dynamic_data)

        udp_socket.sendto(horn_byte_data, (RUN8_IP, RUN8_PORT))
        udp_socket.sendto(bell_byte_data, (RUN8_IP, RUN8_PORT))
        udp_socket.sendto(sand_byte_data, (RUN8_IP, RUN8_PORT))
        udp_socket.sendto(autoBrake_byte_data, (RUN8_IP, RUN8_PORT))
        udp_socket.sendto(indyBrake_byte_data, (RUN8_IP, RUN8_PORT))
        udp_socket.sendto(indyBail_byte_data, (RUN8_IP, RUN8_PORT))
        udp_socket.sendto(throttle_byte_data, (RUN8_IP, RUN8_PORT))
        udp_socket.sendto(reverser_byte_data, (RUN8_IP, RUN8_PORT))
        udp_socket.sendto(dynamic_byte_data, (RUN8_IP, RUN8_PORT))

except serial.SerialException as e:
    print("Serial error:", e)
except KeyboardInterrupt:
    print("Exiting...")
finally:
    udp_socket.close()    


