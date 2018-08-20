# -*- coding: utf-8 -*-

# GUI
from tkinter import *
from tkinter import ttk

# 画像処理
from PIL import Image
from pyzbar import pyzbar

# ------------------------------
# クラス定義
# ------------------------------
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
        
    # デバッグ用(コンソールにフィールドを展開)
    def showFieldList(self):
        for i in range(self.height):
            for j in range(self.width):
                print(str("{0:2d}".format(self.sqs[i][j].getPoint())) + " ", end="")
            print("\n")


# ------------------------------
# 関数定義 
# ------------------------------

# QRコードのデータを数値配列に変換
def dataParse(qdata):
    # データを分割して余分な空白を取り除く
    data = qdata.split(':')
    data.remove("")

    for i in range(len(data)):
        # データを1つずつのint値に変換してリスト化
        data[i] = list(map(int, data[i].split()))
    
    return data

def showQRwindow(fldfrm):
    subWindow = Toplevel()

    btnfrm = QRButtonFrame(fldfrm, subWindow)
    btnfrm.grid(row=0, column=0)

# ------------------------------
# フレーム定義
# ------------------------------
class FieldFrame(Frame):
    field = None
    genButton = None

    def init(self):
        self.genButton = ttk.Button(
            self,
            text="Generate Field",
            command=self.generateField,
            )
        self.genButton.grid(row=0, column=0)

    def __init__(self, master = None):
        self.master = master
        Frame.__init__(self, master)
        self.grid(row=0, column=0)
        self.init()

    def generateField(self):
        self.genButton.destroy()
        self.showField()

    def showField(self):
        squareLabel = [[None for i in range(self.field.width)] for j in range(self.field.height)]
        for i in range(self.field.height):
            for j in range(self.field.width):
                temp = self.field.sqs[i][j].getPoint()
                squareLabel[i][j] = ttk.Label(
                    self,
                    text = str(temp),
                    background = "#ffffff",
                    padding = (5, 10))
                squareLabel[i][j].grid(row=i+1, column=j+1)

class QRButtonFrame(Frame):
    nextfrm = None
    master = None

    def init(self):
        button = ttk.Button(
                self, 
                text="read QRcode", 
                command = self.pushed
                )
        button.grid(row=0, column=0)

    def __init__(self, nextfrm, master = None):
        self.nextfrm = nextfrm
        self.master = master
        Frame.__init__(self, master)
        self.grid(row=0, column=0)
        self.init()

    def pushed(self):
        qrimg = Image.open('../img/qrsample.png')
        fdata = pyzbar.decode(qrimg)[0][0].decode('utf-8')
        self.nextfrm.field = Field(dataParse(fdata))
        self.master.withdraw()

def main():
    root = Tk()
    root.title("Procon29 solver"); root.geometry("640x480")
    root.grid_rowconfigure(0, weight=1)
    root.grid_columnconfigure(0, weight=1)

    fldfrm = FieldFrame(root)
    fldfrm.grid(row=0, column=0)

    showQRwindow(fldfrm)
    
    field = None

    root.mainloop()

if __name__ == "__main__":
    main()
