# -*- coding: utf-8 -*-

# GUI
from tkinter import *
from tkinter.ttk import *

# 画像処理
from PIL import Image, ImageTk
from pyzbar import pyzbar
import cv2

# その他
import numpy as np
import sys # CLI引数の受け取り

# ------------------------------
# GUIパーツ
# ------------------------------
FREE, OWN, ENEMY = 0, 1, 2

class Square:
    label = None
    _state = FREE
    _point = 0
    _player = 0
    _field = None
    
    def __init__(self, value, master, field):
        self.setPoint(value)
        self.setField(field)
        self.label = Label(
            master,
            text = str(self.getPoint()),
            foreground = self.getFg(),
            background = self.getColor(),
            borderwidth = 3,
            relief = "ridge",
            font = ("Migu 1C", 18),
            width = 2,
            padding = (2, 2)
            )
        self.label.bind("<1>", self.__Lclicked)
        self.label.bind("<2>", self.__Mclicked)
        self.label.bind("<3>", self.__Rclicked)

    # 左クリックで自陣操作
    def __Lclicked(self, event):
        # エージェントのいる位置を初めてクリックしたとき
        if self.getState() == OWN and self.getField().clicked == -1:
            # クリックされたマスを保存
            self.getField().clicked = self
        # 既にエージェントがクリックされた後の場合
        elif self.getField().clicked != -1:
            if self.getField().clicked == self:
                self.getField().clicked = -1

            # 移動先が自陣
            elif self.getState() == OWN:
                self.setFg(self.getField().clicked.getPlayer())
                self.getField().clicked.setFg(0)
                self.getField().clicked = -1
            # 指定先が敵陣
            elif self.getState() == ENEMY:
                self.setState(FREE)
                self.getField().clicked = -1
            # 指定先が空白
            else:
                self.setState(OWN)
                self.setFg(self.getField().clicked.getPlayer())
                self.getField().clicked.setFg(0)
                self.getField().clicked = -1

    # 右クリックで敵陣操作
    def __Rclicked(self, event):
        if self.getState() == FREE:
            self.setState(ENEMY)
        else:
            self.setState(FREE)

    # ミスったときの救済
    def __Mclicked(self, event):
        if self.getState() == FREE:
            self.setState(OWN)
        else:
            self.setState(FREE)

    # getter
    def getField(self):
        return self._field
    def getPoint(self):
        return self._point
    def getState(self):
        return self._state
    def getColor(self):
        if self._state == FREE:
            return "#202020"
        elif self._state == OWN:
            return "#1F6764"
        elif self._state == ENEMY:
            return "#C94D65"
    def getPlayer(self):
        return self._player
    def getFg(self):
        if self._player == 1:
            return "#888888"
        elif self._player == 2:
            return "#F58888"
        else:
            return "#ffffff"

    # setter
    def setField(self, value):
        self._field = value
    def setPoint(self, value):
        self._point = value
    def setState(self, value):
        self._state = value
        self.label["background"] = self.getColor()
    def setFg(self, value):
        self._player = value
        self.label["foreground"] = self.getFg()

class Field:
    height, width = 12, 12
    sqs = [[0 for i in range(12)] for j in range(12)] 

    pown = [None, None]
    pene = [None, None]
    clicked = -1
    
    def __init__(self, data, master):
        self.genField(data, master)

    def genField(self, data, master):
        self.height, self.width = data[0][0], data[0][1]

        for i in range(self.height):
            for j in range(self.width):
                self.sqs[i][j] = Square(data[i+1][j], master, self)
        
        # 自エージェントの初期位置を取得
        sdefault = [[data[self.height+1][0], data[self.height+1][1]], \
                     [data[self.height+2][0], data[self.height+2][1]]]

        # 初期陣地を設定
        self.pown[0] = self.sqs[sdefault[0][0] - 1][sdefault[0][1] - 1]
        self.pown[0].setState(OWN)
        self.pown[0].setFg(1)
        self.pown[1] = self.sqs[sdefault[1][0] - 1][sdefault[1][1] - 1]
        self.pown[1].setState(OWN)
        self.pown[1].player = 1
        self.pown[1].setFg(2)

class GearSwitch:
    gear = [0, 0]
    switchTitle = [None for i in range(2)]
    gearButton = [[None for i in range(3)] for j in range(2)]

    def __init__(self, master):
        for i in range(2):
            self.switchTitle[i] = Label(
                master,
                text = "Gear Player" + str(i + 1),
                )
            self.switchTitle[i].grid()
            for j in range(3):
                self.gearButton[i][j] = GearButton(self, master, i, j)

    def clicked(self, player, pushedIndex):
        if self.gear[player] != pushedIndex:
            self.gearButton[player][self.gear[player]].button["text"] = "mode" + str(self.gear[player] + 1)
            self.gearButton[player][self.gear[player]].button.state(['!pressed'])
            self.gearButton[player][pushedIndex].button["text"] = "mode" + str(pushedIndex + 1) + " *"
            self.gearButton[player][pushedIndex].button.state(['pressed'])
            self.gear[player] = pushedIndex

class GearButton:
    def __init__(self, switch, master, player, index):
        self.switch = switch
        self.index = index
        self.player = player
        self.button = Button(
            master,
            text = "mode" + str(index+1),
            )
        self.button.bind("<1>", self.__clicked)
        if index == 0:
            self.button.state(['pressed'])
        self.button.grid()

    def __clicked(self, event):
        self.switch.clicked(self.player, self.index)
    
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

# QRコードを読む際のコントラスト調整
def edit_contrast(image, gamma):
    look_up_table = [np.uint8(255.0 / (1 + np.exp(-gamma * (i - 128.) / 255.)))
        for i in range(256)]
 
    result_image = np.array([look_up_table[value]
                             for value in image.flat], dtype=np.uint8)
    result_image = result_image.reshape(image.shape)
    return result_image
 
def readQR(nextfrm):
    capture = cv2.VideoCapture(0)
    if capture.isOpened() is False:
        raise("IO Error")
 
    while True:
        ret, frame = capture.read()
        if ret == False:
            continue
        
        # グレースケール化してコントラストを調整する
        gray_scale = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        image = edit_contrast(gray_scale, 5)
 
        # 加工した画像からフレームQRコードを取得してデコードする
        codes = pyzbar.decode(image)
 
        if len(codes) > 0:
            fdata = codes[0][0].decode('utf-8')
            nextfrm.field = Field(dataParse(fdata), nextfrm)
            nextfrm.showField()
            break

# ------------------------------
# フレーム定義
# ------------------------------
# 左側の領域
class DetailFrame(Frame):
    currentTurn = 1 # 現在のターン
    maxTurn = 60 # 総ターン数

    def init(self):
        self.turnLabel = Label(
            self,
            text=str(self.currentTurn) + "/" + str(self.maxTurn),
            )
        self.turnLabel.grid(row=0, column=0)
        self.gearSwitch = GearSwitch(self)

    def __init__(self, turnNum, master = None):
        self.master = master
        self.maxTurn = turnNum
        Frame.__init__(self, master)
        self.grid(row=0, column=0)
        self.init()

# 右側の領域
class FieldFrame(Frame):
    field = None

    def __init__(self, master = None):
        self.master = master
        Frame.__init__(self, master)
        self.grid(row=0, column=0)

    def showField(self):
        for i in range(self.field.height):
            for j in range(self.field.width):
                self.field.sqs[i][j].label.grid(row=i+1, column=j+1)

# ------------------------------
# main
# ------------------------------
def main():
    args = sys.argv

    root = Tk()
    root.title("Lexus - Procon29 solver"); root.geometry("640x480")
    root.grid_rowconfigure(0, weight=2)
    root.grid_columnconfigure(0, weight=2)
    root.configure(bg="#161616")

    style = Style()
    style.configure(".", font=("Migu 1C", 10))
    style.configure(".", foreground = "#ffffff")
    style.configure(".", background = "#161616")

    fldfrm = FieldFrame(root)
    readQR(fldfrm)
    fldfrm.grid(row=0, column=1)

    dtlfrm = DetailFrame(args[1], root)
    dtlfrm.grid(row=0, column=0)

    root.mainloop()

if __name__ == "__main__":
    main()
