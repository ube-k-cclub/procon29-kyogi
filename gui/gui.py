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
FREE, OWN, ENEMY = 0, 1, 2

class Square:
    _state = FREE
    _point = 0
    label = None
    
    def __init__(self, value, master):
        self.setPoint(value)
        self.label = ttk.Label(
            master,
            text = str(self.getPoint()),
            background = self.getColor(),
            padding = (5, 10))
        self.label.bind("<1>", self.__clicked)

    def __clicked(self, event):
        if self.getState() == FREE:
            self.setState(OWN)
        elif self.getState() == OWN:
            self.setState(FREE)


    # getter
    def getPoint(self):
        return self._point
    def getState(self):
        return self._state
    def getColor(self):
        if self._state == FREE:
            return "#ffffff"
        elif self._state == OWN:
            return "#87ceeb"
        elif self._state == ENEMY:
            return "#ffb6c1"

    # setter
    def setPoint(self, value):
        self._point = value
    def setState(self, value):
        self._state = value
        self.label["background"] = self.getColor()

class Field:
    height, width = 12, 12
    sqs = [[0 for i in range(12)] for j in range(12)] 
    
    def __init__(self, data, master):
        self.genField(data, master)

    def genField(self, data, master):
        self.height, self.width = data[0][0], data[0][1]

        for i in range(self.height):
            for j in range(self.width):
                self.sqs[i][j] = Square(data[i+1][j], master)
        
        # 自エージェントの初期位置を取得
        sdefault = [[data[self.height+1][0], data[self.height+1][1]], \
                     [data[self.height+2][0], data[self.height+2][1]]]

        # 両チームの初期陣地を設定
        self.sqs[sdefault[0][0] - 1][sdefault[0][1] - 1].setState(OWN)
        self.sqs[sdefault[1][0] - 1][sdefault[1][1] - 1].setState(OWN)

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
            command=self.showField,
            )
        self.genButton.grid(row=0, column=0)

    def __init__(self, master = None):
        self.master = master
        Frame.__init__(self, master)
        self.grid(row=0, column=0)
        self.init()

    def showField(self):
        self.genButton.destroy()
        for i in range(self.field.height):
            for j in range(self.field.width):
                self.field.sqs[i][j].label.grid(row=i+1, column=j+1)

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
        self.nextfrm.field = Field(dataParse(fdata), self.nextfrm)
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
