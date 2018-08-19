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
    
    def __init__(self, value):
        self.setPoint(value)

    # getter
    def getPoint(self):
        return self.point
    def getState(self):
        return self.state

    # setter
    def setPoint(self, value):
        self.point = value
    def setState(self, value):
        self.state = value

class Field:
    height = 12; width = 12
    sqs = [[0 for i in range(12)] for j in range(12)] 
    def __init__(self, data):
        self.genField(data)

    def genField(self, data):
        self.height = data[0][0]; self.width = data[0][1]
        for i in range(self.height):
            for j in range(self.width):
                self.sqs[i][j] = Square(data[i+1][j])
        
    def showFieldList(self):
        for i in range(self.height):
            for j in range(self.width):
                print(str("{0:2d}".format(self.sqs[i][j].getPoint())) + " ", end="")
            print("\n")

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
    return data

def fieldGenerate(data):
    field = Field(qrDataParse(data))
    field.showFieldList()
    
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
