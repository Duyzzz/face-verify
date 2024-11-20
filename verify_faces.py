import json
import sys
from PIL import Image
import cv2
import time

import face_recognition

def faceEncodeInitialize(images):
    LenOfList = len(images)
    for i in range (LenOfList):
        img = cv2.imread(images[i]["imagePath"])
        rbg_img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        images[i]["encode"] = face_recognition.face_encodings(rbg_img)[0]


def verify_faces(image_data, unknown_image_path):

    # Load the image to verify (unknown)
    unknown_image = face_recognition.load_image_file(unknown_image_path)
    unknown_encoding = face_recognition.face_encodings(unknown_image)[0]

    # Compare the two face encodings
    results = face_recognition.compare_faces(image_data, unknown_encoding)

    if True in results:
        return results.index(True)
    else:
        return "not match"
    

if __name__ == "__main__":
    # First argument is known image, second is the loaded image
    storesTime = time.time()
    referenceImage = []
    testFace = "image.jpg"
    with open("referenceData.json", "r") as file:
        referenceImage = json.load(file)
    faceEncodeInitialize(referenceImage)
    referenceImageEncode = [item["encode"] for item in referenceImage]
    print((time.time() - storesTime))
    storesTime = time.time()
   # verify_faces(sys.argv[1], sys.argv[2])

   
    result = verify_faces(referenceImageEncode, testFace)
    if result == "not match":
        print("does not match any reference faces")
    else:
        print(str(referenceImage[result]["name"]) + " verified")
    print((time.time() - storesTime))
