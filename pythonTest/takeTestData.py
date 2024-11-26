import face_recognition
path = "C:\\Users\\DELL\\Pictures\\Camera Roll\\WIN_20231214_00_16_49_Pro.jpg"

loadFile = face_recognition.load_image_file(path)
encoder = face_recognition.face_encodings(loadFile)
print(encoder)