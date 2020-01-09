import cv2
import os
import threading
import ftplib


def get_video_writer(video_index):
    return cv2.VideoWriter(
        get_video_name(video_index),
        cv2.VideoWriter_fourcc(*'mp4v'),
        webcam.get(cv2.CAP_PROP_FPS),
        (width, height)
    )


def get_video_name(video_index):
    return 'output_' + str(video_index) + '.mp4'


def upload_video(video_name):
    ftp = ftplib.FTP(os.environ['FTP_IP'])
    ftp.login(os.environ['FTP_USER'], os.environ['FTP_PSWD'])
    with open(video_name, 'rb') as f:
        ftp.storbinary('STOR ' + '/RaspberryPi/' + video_name, f)
    ftp.quit()

    os.system("rm " + video_name)


width, height = (960, 720)

index = 0

webcam = cv2.VideoCapture(1)
webcam.set(cv2.CAP_PROP_FRAME_WIDTH, width)
webcam.set(cv2.CAP_PROP_FRAME_HEIGHT, height)

output = get_video_writer(index)

# maxDurationOfRecordInSecond = webcam.get(cv2.CAP_PROP_FPS) * 60 * 1  # 60 secondes * 1 = 1 minutes
maxDurationOfRecordInSecond = webcam.get(cv2.CAP_PROP_FPS) * 60 * 30

frameCounter = 0
while webcam.isOpened():
    ret, frame = webcam.read()
    frameCounter += 1

    output.write(frame)

    cv2.imshow('frame', frame)

    if frameCounter >= maxDurationOfRecordInSecond:
        frameCounter = 0
        output.release()

        threading.Thread(target=upload_video, args=(get_video_name(index),)).start()

        index += 1
        output = get_video_writer(index)

    if cv2.waitKey(1) & 0xFF == ord('x'):
        break

webcam.release()
output.release()
cv2.destroyAllWindows()
