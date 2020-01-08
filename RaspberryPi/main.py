import time
import cv2
import os
import threading


def get_video_writer(video_index):
    return cv2.VideoWriter(
        get_video_name(video_index),
        cv2.VideoWriter_fourcc(*'mp4v'),
        webcam.get(cv2.CAP_PROP_FPS),
        (width, height)
    )


def get_video_name(video_index):
    return 'output_' + str(video_index) + '.mp4'


def encode_video(video_name):
    os.system("ffmpeg -y -i " + video_name + " -vcodec libx264 light_" + get_video_name(index))
    os.system("rm " + video_name)


width, height = (960, 720)

index = 0

webcam = cv2.VideoCapture(2)
webcam.set(cv2.CAP_PROP_FRAME_WIDTH, width)
webcam.set(cv2.CAP_PROP_FRAME_HEIGHT, height)

output = get_video_writer(index)

maxDurationOfRecordInSecond = webcam.get(cv2.CAP_PROP_FPS) * 60 * 10  # 60 secondes * 10 = 10 minutes

frameCounter = 0
while webcam.isOpened():
    ret, frame = webcam.read()
    frameCounter += 1

    output.write(frame)

    cv2.imshow('frame', frame)

    if frameCounter >= maxDurationOfRecordInSecond:
        frameCounter = 0
        output.release()

        threading.Thread(target=encode_video, args=(get_video_name(index),)).start()

        index += 1
        output = get_video_writer(index)

    if cv2.waitKey(1) & 0xFF == ord('x'):
        break

webcam.release()
output.release()
cv2.destroyAllWindows()
