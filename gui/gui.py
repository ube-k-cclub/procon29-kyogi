# -*- coding: utf-8 -*-

# GUI
from tkinter import *
from tkinter.ttk import *

# 画像処理
from PIL import Image
from pyzbar import pyzbar

# ------------------------------
# クラス定義
# ------------------------------
FREE, OWN, ENEMY = 0, 1, 2
mode = OWN

class Square:
    _state = FREE
    _point = 0
    label = None
    
    def __init__(self, value, master):
        self.setPoint(value)
        self.label = Label(
            master,
            text = str(self.getPoint()),
            foreground = "#ffffff",
            background = self.getColor(),
            font = ("Migu 1C", 18),
            width = 2,
            padding = (5, 5)
            )
        self.label.bind("<1>", self.__clicked)

    def __clicked(self, event):
        if self.getState() == FREE:
            self.setState(mode)
        else:
            self.setState(FREE)


    # getter
    def getPoint(self):
        return self._point
    def getState(self):
        return self._state
    def getColor(self):
        if self._state == FREE:
            return "#161616"
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

def showQRwindow(swtfrm):
    subWindow = Toplevel()

    btnfrm = QRButtonFrame(swtfrm, subWindow)
    btnfrm.grid(row=0, column=0)

# ------------------------------
# フレーム定義
# ------------------------------
class FieldFrame(Frame):
    field = None
    genButton = None

    def init(self):
        self.genButton = Button(
            self,
            text="Generate Field",
            command=self.showField,
            )
        self.genButton.grid(row=0, column=0)

    def __init__(self, master = None):
        self.master = master
        Frame.__init__(self, master)
        self['height'] = 480
        self['width'] = 640
        # self['background'] = "#161616"
        self.grid(row=0, column=0)
        self.init()

    def showField(self):
        self.genButton.destroy()
        for i in range(self.field.height):
            for j in range(self.field.width):
                self.field.sqs[i][j].label.grid(row=i+1, column=j+1)

class SwitchFrame(Frame):
    modeLabel = None

    def __initWidget(self):
        self.modeLabel = Label(
            self,
            text = str(mode),
            )
        self.modeLabel.bind("<1>", self.__changeMode)
        self.modeLabel.grid(row=0, column=0)

    def __init__(self, master = None):
        Frame.__init__(self, master)
        self.grid(row=0, column=0)
        self.__initWidget()

    def __changeMode(self, event):
        global mode
        if mode == OWN:
            mode = ENEMY  
        else:
            mode = OWN

        self.modeLabel["text"] = mode

class QRButtonFrame(Frame):
    nextfrm = None
    master = None

    def init(self):
        button = Button(
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
    root.grid_rowconfigure(0, weight=2)
    root.grid_columnconfigure(0, weight=2)
    root.configure(bg="#161616")

    style = Style()
    style.configure(".", font=("Migu 1C", 10))
    style.configure(".", foreground = "#ffffff")
    style.configure(".", background = "#161616")

    swtfrm = FieldFrame(root)
    swtfrm.grid(row=1, column=1)

    modfrm = SwitchFrame(root)
    modfrm.grid(row=1, column=0)

    showQRwindow(swtfrm)
    
    field = None

    root.mainloop()

if __name__ == "__main__":
    main()
