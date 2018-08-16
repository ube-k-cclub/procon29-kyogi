# -*- coding: utf-8 -*-

# GUI
from tkinter import *

# 画像処理
from PIL import Image
from pyzbar import pyzbar

def QRpushed():
    qrimg = Image.open('img/qrsample.png')
    strings = pyzbar.decode(qrimg)
    print(strings)
    
def init():
    root = Tk()
    root.title("Procon29 solver")
    root.geometry("640x480")

    button = Button(
            root, 
            text="read QRcode", 
            command = QRpushed
            )
    button.pack()

    return root

if __name__ == "__main__":
    root = init()
    root.mainloop()
