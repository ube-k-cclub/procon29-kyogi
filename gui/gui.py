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
import os # ソルバの実行

gear = [0, 0]
field = None
# ------------------------------
# GUIパーツ
# ------------------------------
FREE, OWN, ENEMY = 0, 1, 2
OWN1, OWN2, ENEMY1, ENEMY2 = 1, 2, 3, 4

class Square:
    global field
    label = None
    _state = FREE
    _point = 0
    _player = 0
    
    def __init__(self, value, master):
        self.setPoint(value)
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
        if self.getState() == OWN and field.clicked == -1:
            # クリックされたマスを保存
            field.clicked = self
        # 既にエージェントがクリックされた後の場合
        elif field.clicked != -1:
            if field.clicked == self:
                field.clicked = -1

            # 移動先が自陣
            elif self.getState() == OWN:
                self.setFg(field.clicked.getPlayer())
                field.clicked.setFg(0)
                field.clicked = -1
            # 指定先が敵陣
            elif self.getState() == ENEMY:
                self.setState(FREE)
                self.getField().clicked = -1
            # 指定先が空白
            else:
                self.setState(OWN)
                self.setFg(field.clicked.getPlayer())
                field.clicked.setFg(0)
                field.clicked = -1

    # 右クリックで敵陣操作
    def __Rclicked(self, event):
        # 最初の2回は開始位置の指定を含める
        if field.startpoint == 2: 
            self.setState(ENEMY)
            self.setFg(ENEMY1)
            field.startpoint -= 1
        elif field.startpoint == 1:
            self.setState(ENEMY)
            self.setFg(ENEMY2)
            field.startpoint -= 1
            
        else: # 通常移動
            # エージェントのいる位置を初めてクリックしたとき
            if self.getState() == ENEMY and field.clicked == -1:
                # クリックされたマスを保存
                field.clicked = self
            # 既にエージェントがクリックされた後の場合
            elif field.clicked != -1:
                if field.clicked == self:
                    field.clicked = -1

                # 移動先が敵陣
                elif self.getState() == ENEMY:
                    self.setFg(field.clicked.getPlayer())
                    field.clicked.setFg(0)
                    field.clicked = -1
                # 指定先が自陣
                elif self.getState() == OWN:
                    self.setState(FREE)
                    field.clicked = -1
                # 指定先が空白
                else:
                    self.setState(ENEMY)
                    self.setFg(field.clicked.getPlayer())
                    field.clicked.setFg(0)
                    field.clicked = -1

    # ミスったときの救済
    def __Mclicked(self, event):
        if self.getState() == FREE:
            self.setState(OWN)
        elif self.getState() == OWN:
            self.setState(ENEMY)
        else:
            self.setState(FREE)

    # getter
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
        elif self._player == 3 or self._player == 4:
            return "#8888F5"
        else:
            return "#ffffff"

    # setter
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
    startpoint = 2
    sqs = [[0 for i in range(12)] for j in range(12)] 

    pown = [None, None]
    pene = [None, None]
    clicked = -1

    ideal = [[0 for i in range(4)] for j in range(2)]
    
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

        # 初期陣地を設定
        self.pown[0] = self.sqs[sdefault[0][0] - 1][sdefault[0][1] - 1]
        self.pown[0].setState(OWN)
        self.pown[0].setFg(1)
        self.pown[1] = self.sqs[sdefault[1][0] - 1][sdefault[1][1] - 1]
        self.pown[1].setState(OWN)
        self.pown[1].player = 1
        self.pown[1].setFg(2)

class GearSwitch:
    global gear
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
        if gear[player] != pushedIndex:
            self.gearButton[player][gear[player]].button["text"] = "mode" + str(gear[player] + 1)
            self.gearButton[player][gear[player]].button.state(['!pressed'])
            self.gearButton[player][pushedIndex].button["text"] = "mode" + str(pushedIndex + 1) + " *"
            self.gearButton[player][pushedIndex].button.state(['pressed'])
            gear[player] = pushedIndex

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
    global field
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
            field = Field(dataParse(fdata), nextfrm)
            nextfrm.showField()
            break

def readOutput(dtlfrm):
    f = open('output.txt', 'r')
    data = f.readlines()

    for d in data:
        d.rstrip()

    # 1行目：自パネルの合計点
    dtlfrm.panelp[OWN-1] = int(data[0])

    # 2行目：自領域合計点
    dtlfrm.areap[OWN-1] = int(data[1])

    # 4行目：敵パネル合計
    dtlfrm.panelp[ENEMY-1] = int(data[3])
    
    # 5行目：自領域合計点
    dtlfrm.areap[ENEMY-1] = int(data[4])
    
    # 7~10行目：P1理想手
    for i in range(4):
        field.ideal[0][i] = int(data[6+i])

    # 11~14行目：P2理想手
    for i in range(4):
        field.ideal[1][i] = int(data[10+i])

    f.close()

def makeSetUp(turn):
    global field

    f = open('setup.txt', 'w')

    # 1行目：ターン数
    f.write(str(turn) + "\n")
    # 2行目：width 3行目：height
    f.write(str(field.width) + "\n")
    f.write(str(field.height) + "\n")
    # 4行目：プレイヤーカラー
    f.write(str(OWN) + "\n")
    # 5行目〜：パネルのポイント
    for i in range(field.height):
        for j in range(field.width):
            f.write(str(field.sqs[i][j].getPoint()) + "\n")

    f.close()

def makeConnect(turn):
    global field
    global gear

    f = open("connect.txt", "w")

    agents = [0,0,0,0]

    # 1〜4行目：red1,2,blue1,2
    for i in range(field.height):
        for j in range(field.width):
            if field.sqs[i][j].getPlayer() != 0:
                agents[field.sqs[i][j].getPlayer()-1] = i*field.width + j

    for i in agents:
        f.write(str(i) + "\n")

    # 5,6行目：ギア
    for i in range(2):
        f.write(str(gear[i] + 1) + "\n")

    # 7行目：現在ターン
    f.write(str(turn) + "\n")

    # 8行目〜：パネルカラー
    for i in range(field.height):
        for j in range(field.width):
            f.write(str(field.sqs[i][j].getState()) + "\n")

    f.close()

# ------------------------------
# フレーム定義
# ------------------------------
# 左側の領域
class DetailFrame(Frame):
    currentTurn = 1 # 現在のターン
    maxTurn = 60 # 総ターン数

    # ポイントを自陣、敵陣の順に格納
    panelp = [0, 0]
    areap = [0, 0]

    def init(self):
        # ターン数表示
        self.turnLabel = Label(
            self,
            text=str(self.currentTurn) + "/" + str(self.maxTurn),
            )
        self.turnLabel.grid(row=0, column=0)

        # ポイント表示
        self.ownPLabel = Label(
            self,
            )
        self.enePLabel = Label(
            self,
            )
        self.getPoint()
        self.ownPLabel.grid()
        self.enePLabel.grid()

        # ギアスイッチ表示
        gearSwitch = GearSwitch(self)
        self.enterButton = Button(
            self,
            text = "Solve",
            )
        self.enterButton.grid()
        self.enterButton.bind("<1>", self.connect)

    def __init__(self, turnNum, master = None):
        self.master = master
        self.maxTurn = turnNum
        Frame.__init__(self, master)
        self.grid(row=0, column=0)
        self.init()

    def connect(self, event):
        self.turnLabel["text"] = str(self.currentTurn) + "/" + str(self.maxTurn)
        makeConnect(self.currentTurn)

        self.currentTurn += 1
        # ソルバを実行する
        os.system('./solver.out')
        # output.txtを読み込んで適用する
        readOutput(self)
        self.getPoint()

    def getPoint(self):
        self.ownPLabel["text"] = "Player " + str(self.panelp[OWN-1]) + "+" + str(self.areap[OWN-1]) + "=" + str(self.panelp[OWN-1] + self.areap[OWN-1])
        self.enePLabel["text"] = "Enemy " + str(self.panelp[ENEMY-1]) + "+" + str(self.areap[ENEMY-1]) + "=" + str(self.panelp[ENEMY-1] + self.areap[ENEMY-1])

# 右側の領域
class FieldFrame(Frame):
    global field

    def __init__(self, master = None):
        self.master = master
        Frame.__init__(self, master)
        self.grid(row=0, column=0)

    def showField(self):
        for i in range(field.height):
            for j in range(field.width):
                field.sqs[i][j].label.grid(row=i+1, column=j+1)

# ------------------------------
# main
# ------------------------------
def main():
    global field
    args = sys.argv
    turnNum = args[1]

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

    makeSetUp(turnNum)

    dtlfrm = DetailFrame(turnNum, root)
    dtlfrm.grid(row=0, column=0)

    root.mainloop()

if __name__ == "__main__":
    main()
