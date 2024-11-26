import socket
import json
from PIL import Image
import cv2
import time
import numpy as np
import projectPyModule.uploadToDrive as ul
import face_recognition
# hostIp at school_Leduy 192.168.217.149
hostIP = "192.168.1.162" # wifi o nha
esp32IP = "192.168.1.139" # wifi o nha
# esp32IP = "192.168.242.96" # wifi cua doan
# hostIP = "192.168.242.149" # wifi doan
storesTime = time.time()
referenceImage = []
referenceImageEncode = []

def faceEncodeInitialize(images):
    LenOfList = len(images)
    for i in range (LenOfList):
        img = cv2.imread(images[i]["imagePath"])
        rbg_img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        images[i]["encode"] = face_recognition.face_encodings(rbg_img)[0]

def verify_faces(image_data, unknown_image_path):

    # Load the image to verify (unknown)
    unknown_image = face_recognition.load_image_file(unknown_image_path)
    try:
        unknown_encoding = face_recognition.face_encodings(unknown_image)[0]
    except IndexError:
        return "not match"    # Compare the two face encodings
    results = face_recognition.compare_faces(image_data, unknown_encoding, tolerance=0.5)

    if True in results:
        return results.index(True)
    else:
        return "not match"
    

def writeObToJson(filePath, ob, imagePath):
    # Read the existing JSON file
    newPerson = {"name": ob, "imagePath":imagePath}
    with open(filePath, 'r') as file:
        data = json.load(file)  # Load JSON into a Python list
    # Add the new object
    data.append(newPerson)
    # Write the updated data back to the file
    with open(filePath, 'w') as file:
        json.dump(data, file, indent=4)
def executeVerifyResult(resultData, addressOfClient):
    print(f"Received complete data of {len(resultData)} bytes from {addressOfClient}")
    # print(complete_data)
    # Send final confirmation
    image = cv2.imdecode(np.frombuffer(resultData, dtype=np.uint8), cv2.IMREAD_COLOR)

# Convert BGR to RGB
    # image_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    image = cv2.flip(image, -1)
    # print(image_rgb.shape)
    # cv2.imwrite("image_captures_data\\image.jpg", image)
    result = verify_faces(referenceImageEncode, "image_captures_data\\image.jpg")
    if result == "not match":
        print("does not match any reference faces")
    else:
        print(str(referenceImage[result]["name"]) + " verified")

def start_udp_server(host=hostIP, port=3333):
    """
    Starts a UDP server that listens on the specified host and port.
    :param host: The host to bind the server to (default: "0.0.0.0").
    :param port: The port to listen on (default: 3333).
    """
    imageIndex = 0

    # Create a UDP socket
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        # Bind the socket to the specified address and port
        server_socket.bind((host, port))
        print(f"UDP server listening on {host}:{port}")
        # while initESP == False or initCSharp == False:
        #     data_init, address_init = server_socket.recvfrom(1024)
        #     value = ''.join(chr(num) for num in data_init[0:10])
        #     if(value == "esp"):
        #         esp32Address = address_init
        #         print("get esp address at: " + str(address_init))
        #         initESP = True
        #     if(value == "CSharp"):
        #         cSharpAddress = address_init
        #         print("get c# address at: " + str(address_init))
        #         initCSharp = True
        # server_socket.sendto(b"get address success", (esp32IP, 12345))
        
        while True:
            print("checking")
            save_but = True
            try:
                # Initialize an empty bytearray to store the complete data
                
                # Receive data chunks until we get an empty packet
                while True:
                    print("hello")
                    if(save_but == True):
                        data, client_address = server_socket.recvfrom(1024)
                        print("client address: " + str(client_address))
                        command = ''.join(chr(num) for num in data[0:15])
                    elif(save_but == False):
                        command = "adding"
                    if(command == "fail"):
                        break
                    elif(command == "verify"):
                        # require for new image captured
                        print("check sent verify")
                        complete_data = bytearray()
                        server_socket.sendto(b'capture image',  (esp32IP, 12345))
                        timeout = 0
                        server_socket.settimeout(5.0)
                        while True:
                            try: 
                                image_data, client_address_data = server_socket.recvfrom(1024)
                                subCommand = ''.join(chr(num) for num in image_data[0:15])
                                # print("data len: " + str(len(image_data)))
                                if(subCommand == "successful"):
                                    print("check successful data")
                                    break
                                elif(subCommand == "fail"):
                                    print("data sending fail")
                                    break
                                if not image_data:
                                    break
                                complete_data.extend(image_data)
                            except socket.timeout:
                                timeout = -1
                                print("sending image fail, please check internet connection")
                                break
                        server_socket.settimeout(None)
                        if(timeout == -1):
                            server_socket.sendto(b'f,f', client_address)
                        else:
                            image = cv2.imdecode(np.frombuffer(complete_data, dtype=np.uint8), cv2.IMREAD_COLOR)
                            image = cv2.flip(image, -1)
                            # Hiển thị hình ảnh trong cửa sổ
                            # cv2.imshow('Image Window', image)
                            # cv2.waitKey(5000)
                            # cv2.destroyAllWindows()
                            imagePath = f"image_captures_data\\image_{imageIndex}.jpg"
                            imageIndex += 1
                            cv2.imwrite(imagePath, image)
                            print("save file")
                            result = verify_faces(referenceImageEncode, imagePath)
                            print(result)
                            if result == "not match":
                                print("does not match any reference faces")
                                t = ("n," + imagePath).encode("utf-8")
                                server_socket.sendto(t, client_address)
                                ul.upload_photo(f"notMatch{imageIndex}", imagePath, verified=False)
                            else:
                                print(str(referenceImage[result]['name']) + " verified")
                                t = (str(referenceImage[result]['name']) + "," + imagePath).encode("utf-8")
                                server_socket.sendto(t, client_address)
                            ul.upload_photo(f"match{imageIndex}_{referenceImage[result]['name']}", imagePath, verified=True)
                            break
                        # executeVerifyResult(oldImage, oldClientAddress)
                    elif(command == "adding"):
                        print("check sent adding")
                        save_but = True
                        complete_data = bytearray()
                        server_socket.sendto(b'capture image',  (esp32IP, 12345))
                        server_socket.settimeout(5.0)  
                        timeout = 0
                        while True:
                            try:
                                image_data, client_address_data = server_socket.recvfrom(1024)
                                subCommand = ''.join(chr(num) for num in image_data[0:15])
                                # print("data len: " + str(len(image_data)))
                                if(subCommand == "successful"):
                                    print("check successful data adding")
                                    break
                                if not image_data:
                                    break
                                complete_data.extend(image_data)
                            except socket.timeout:
                                print("time out")
                                timeout = -2
                                break
                        if(timeout != -2):
                            image = cv2.imdecode(np.frombuffer(complete_data, dtype=np.uint8), cv2.IMREAD_COLOR)
                            image = cv2.flip(image, -1)
                            cv2.imwrite("D:\\university\\ky7_zz\\doAnDoLuong\\code_main\\imageSaveTemp\\temp.jpg", image)
                            print("delay a little")
                            imageToSave = face_recognition.load_image_file("D:\\university\\ky7_zz\\doAnDoLuong\\code_main\\imageSaveTemp\\temp.jpg")
                            face_location = face_recognition.face_locations(imageToSave)
                        if(timeout == -2):
                            temp = b'f,f'
                        if(face_location):
                            temp = b'y,y'
                        else:
                            temp = b'n,n'

                        client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                        client_socket.sendto(temp, (hostIP, 6523))
                        client_socket.close()
                        
                        server_socket.settimeout(None)  # Switch back to blocking mode
                        image_data, client_address_data = server_socket.recvfrom(1024)
                        subCommand = ''.join(chr(num) for num in image_data[0:15])
                        if(subCommand == "adding"):
                            save_but = False
                            print("check1")
                        else:
                            path = "D:\\university\\ky7_zz\\doAnDoLuong\\code_main\\referenceImages\\" + subCommand + ".jpg"
                            cv2.imwrite(path, image)
                            writeObToJson("referenceData.json", subCommand, path)
                            print("check2")
                            save_but = True
                    # complete_data.extend(data)
                    
                    # Send acknowledgment for each chunk
                    # if(len(data) < 1024):
                    #     executeVerifyResult(complete_data, client_address)
                    #     oldImage = complete_data
                    #     oldClientAddress = client_address
                    #     break
            except ConnectionResetError:
                print("Connection was reset by the client. Continuing to listen...")
                continue

    except KeyboardInterrupt:
        print("\nServer shutting down.")
    finally:
        server_socket.close()

if __name__ == "__main__":
    with open("referenceData.json", "r") as file:
        referenceImage = json.load(file)
    faceEncodeInitialize(referenceImage)
    referenceImageEncode = [item["encode"] for item in referenceImage]
    print((time.time() - storesTime))
    storesTime = time.time()
    start_udp_server()
