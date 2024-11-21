import socket
import json
from PIL import Image
import cv2
import time
import numpy as np

import face_recognition
# hostIp at school_Leduy 192.168.217.149
hostIP = "192.168.1.162" # at home
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
    results = face_recognition.compare_faces(image_data, unknown_encoding)

    if True in results:
        return results.index(True)
    else:
        return "not match"
    

def start_udp_server(host=hostIP, port=3333):
    """
    Starts a UDP server that listens on the specified host and port.
    :param host: The host to bind the server to (default: "0.0.0.0").
    :param port: The port to listen on (default: 3333).
    """
    # Create a UDP socket
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    try:
        # Bind the socket to the specified address and port
        server_socket.bind((host, port))
        print(f"UDP server listening on {host}:{port}")

        while True:
            print("checking")
            try:
                # Initialize an empty bytearray to store the complete data
                complete_data = bytearray()
                
                # Receive data chunks until we get an empty packet
                while True:
                    data, client_address = server_socket.recvfrom(1024)
                    if(data[0] == 102):
                        if(data[1] == 97):
                            if(data[2] == 105):
                                if(data[3] == 108):
                                    print("check fail")
                                    break
                    if not data:  # Empty packet signals end of transmission
                        break
                    complete_data.extend(data)
                    
                    # Send acknowledgment for each chunk
                    if(len(data) < 1024):
                        break
                    # print("data length: " + str(len(data)))
                    # server_socket.sendto(response.encode(), client_address)
                t = time.time()
                print(f"Received complete data of {len(complete_data)} bytes from {client_address}")
                # print(complete_data)
                # Send final confirmation
                image = cv2.imdecode(np.frombuffer(complete_data, dtype=np.uint8), cv2.IMREAD_COLOR)

# Convert BGR to RGB
                # image_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
                image = cv2.flip(image, -1)
                # print(image_rgb.shape)
                cv2.imwrite("image_captures_data\\image.jpg", image)
                result = verify_faces(referenceImageEncode, "image_captures_data\\image.jpg")
                if result == "not match":
                    print("does not match any reference faces")
                else:
                    print(str(referenceImage[result]["name"]) + " verified")
                print("convert and save time: " + str(time.time() - t))
                # server_socket.sendto("data received ok", client_address)
                
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
