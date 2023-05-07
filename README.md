# SaposenBoard_MotorTest

SaposenBoardの動作確認用プログラムです

このプログラムで次の機能を確認できます
- モータードライバー

## Requirement

[Keil Studio Cloud](https://studio.keil.arm.com/)

専用アカウントが必要なので、初めて使う人は[ここ](https://os.mbed.com/accounts/login/)から`Create an account`を選択してアカウント作ってください

## Download

1. [Keil Studio Cloud](https://studio.keil.arm.com/)を開く

2. 画面上部のメニューバーにある`File`→`Clone`を選択

![image](https://user-images.githubusercontent.com/87698678/236663976-31cfa8eb-8b42-475a-a41a-a75607c134f4.png)

3. URL欄にこのリポジトリのURL`https://github.com/hide4096/SaposenBoard_MotorTest`を入力

![image](https://user-images.githubusercontent.com/87698678/236664076-1a32972a-56de-4dd9-baef-e2aac3f22479.png)

4. Cloneボタンを押すとリポジトリのクローンとライブラリの導入が始まるので、しばらく待つ

5. Active projectとBuild targetが画像と同じになっているか確認して、トンカチ(?)のマークを押す

![image](https://user-images.githubusercontent.com/87698678/236664159-783a996e-f9ac-4ce5-8658-3d497ad00ebc.png)

6. `saposenboard_motortest.NUCLEO_F303K8.bin`というファイルがダウンロードされるので、適当なところに保存する

7. SaposenBoardをパソコンに接続する

8. `NODE_F303K8`というUSBメモリが認識されるので、そこに6でダウンロードしたファイルを入れる

## Usage

1. SaposenBoardをパソコンから外す

2. モーターとバッテリーを接続する

3. トグルスイッチをONにする

4. モータが正転逆転を繰り返す

モータが回らない場合は、はんだ付けに不良があります。ドンマイ


