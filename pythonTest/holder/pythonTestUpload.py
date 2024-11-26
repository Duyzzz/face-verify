from googleapiclient.discovery import build
from google.oauth2 import service_account

SCOPES = ['https://www.googleapis.com/auth/drive']
SERVICE_ACCOUNT_FILE = 'service_account.json'
PARENT_FOLDER_ID = "1tlXlivK0ZYbZHVDvB0XnQSwmIOk26B_L"


def authenticate():
    credentials = service_account.Credentials.from_service_account_file(SERVICE_ACCOUNT_FILE)
    return credentials

def upload_photo(file_name, file_path):
    credential = authenticate()
    service = build('drive', 'v3', credentials=credential)

    file_metadata = {
        'name': file_name,
        'parents': [PARENT_FOLDER_ID]
    }
    file = service.files().create(
        body = file_metadata,
        media_body = file_path
    ).execute()

# upload_photo("ok", "D:\\university\\ky7_zz\\doAnDoLuong\\code_main\\image_captures_data\\image_6.jpg")