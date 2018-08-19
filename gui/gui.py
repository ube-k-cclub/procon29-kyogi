# -*- coding: utf-8 -*-

# GUI
from tkinter import *

# 画像処理
from PIL import Image
from pyzbar import pyzbar

class Square:
    FREE = 0; OWN = 1; ENEMY = 2;
    state = FREE
    point = 0;
    
    def __init__():
        pass

class Field:
    sqs = [[0 for i in range(12)] for j in range(12)] 
    def __init__():
        pass
        
def QRpushed():
    qrimg = Image.open('../img/qrsample.png')
    fdata = pyzbar.decode(qrimg)[0][0].decode('utf-8')
    fieldGenerate(fdata) 

# QRコードのデータを数値配列に変換
def qrDataParse(qdata):
    data = qdata.split(':')
    data.remove("")
    for i in range(len(data)):
        data[i] = list(map(int, data[i].split()))
    print(data)

def fieldGenerate(data):
    qrDataParse(data)
    #field = Field()
    
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
