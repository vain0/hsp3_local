;
;	HSP help manager用 HELPソースファイル
;	(先頭が「;」の行はコメントとして処理されます)
;
%type
拡張命令
%ver
3.4
%note
hgimg3.asをインクルードすること。
%date
2014/08/01
%author
onitama
%dll
hgimg3
%url
http://www.onionsoft.net/
%port
Win
%portinfo
Windows+DirectX8以降のシステム上でのみ動作します。

%index
hgini
プラグインの初期化
%group
拡張画面制御命令
%prm
yofs,ysize
yofs    : 有効スクリーン範囲を縦に+yofsだけずらす
ysize   : 有効スクリーン範囲を縦にysizeとする
%inst
HGIMG3の初期化を行ないます。
最初に必ず初期化をしてからHGIMG3の各命令を使用しなければなりません。
hgini命令が実行された時点での描画先(ウィンドウID)が表示先として登録されます。
またその時点での画面初期化モード(ウィンドウモードかフルスクリーンモードか)によって、HGIMG3の描画モードも合わせて設定されます。
yofs,ysizeの指定が省略されるか0の場合はウィンドウ全体が描画対象になります。
%href
hgbye
hgreset


%index
hgreset
シーン情報の初期化
%group
拡張画面制御命令
%prm
%inst
シーンの情報の初期化を行ないます。
すべてのモデル、オブジェクト、テクスチャ、イベントリストの情報は破棄され、まったく新しいシーンの状態に戻します。
%href
hgini
hgbye


%index
hgbye
プラグインの解放
%group
拡張画面制御命令
%prm

%inst
HGIMG3の解放を行ないます。
プログラムの終了時に自動的に実行されるので、通常は明示する必要はありません。
%href
hgini


%index
hgdraw
描画の実行
%group
拡張画面制御命令
%prm
p1
p1(0) : モード
%inst
オブジェクトの全描画処理を実行します。
デフォルトでは、すべてのオブジェクト描画を行ない画面を更新します。
hgdrawは、必ずhgsyncと対にして使用する必要があります。
先にhgdraw命令で描画を行ない、最後にhgsyncで待ち時間を入れるようにしてください。
p1でモードの指定を行なうことができます。
^p
モード  内容
-----------------------------------------------
1       描画エリアを反映する
2       Objectの描画を行なわない
4       画面の初期化を行なわない
^p
モード値は、加算することで複数指定することが可能です。
これにより、１つの画面内を描画エリアごとに描画することが可能になります。

%href
hgini
hgsync
hgview


%index
hgsync
時間待ちを行なう
%group
拡張画面制御命令
%prm
val
val : 待ち時間
%inst
valで指定した時間(ms)だけウェイトを取ります。
hgsyncは、必ずhgdrawと対にして使用する必要があります。
先にhgdraw命令で描画を行ない、最後にhgsyncで待ち時間を入れるようにしてください。
システム変数statに、HGIMG3による負荷(処理にかかったミリ秒単位の時間)を代入します。


%href
getsync
hgdraw


%index
hgsetreq
システムリクエスト設定
%group
拡張画面制御命令
%prm
type,val
type : 設定タイプ(SYSREQ_???)
val  : 設定する値
%inst
HGIMG3に対して様々なシステム設定を行ないます。
type値で指定できるのは以下のシンボルです。
^p
	シンボル名        内容
-----------------------------------------------
	SYSREQ_MAXMODEL   モデル最大数
	SYSREQ_MAXOBJ     オブジェクト最大数
	SYSREQ_MAXTEX     テクスチャ最大数
	SYSREQ_DXMODE     フルスクリーンモードスイッチ
	SYSREQ_DXHWND     ウィンドウハンドル(参照のみ)
	SYSREQ_DXWIDTH    フルスクリーンモード時の横サイズ
	SYSREQ_DXHEIGHT   フルスクリーンモード時の縦サイズ
	SYSREQ_COLORKEY   テクスチャ登録時の透明色コード
	SYSREQ_RESVMODE   エラー発生時の原因コード(参照のみ)
	SYSREQ_MAXEVENT   イベント最大数
	SYSREQ_MDLANIM    モデルあたりのアニメーション最大数
	SYSREQ_CALCNORMAL Xファイルモデル法線再計算スイッチ
	SYSREQ_2DFILTER   2D描画時のテクスチャ補間モード
	SYSREQ_3DFILTER   3D描画時のテクスチャ補間モード
	SYSREQ_OLDCAM     カメラ注視モードの動作
	SYSREQ_QUATALG    Xファイルモデルアニメーション補間モード
	SYSREQ_DXVSYNC    フルスクリーンモード時のVSYNC待ちモード
	SYSREQ_DEFTIMER   hgsyncの時間待ちモード(0=HGIMG3/1=await)
	SYSREQ_NOMIPMAP   テクスチャのMIPMAP生成モード(0=自動/1=MIPMAPなし)
	SYSREQ_DEVLOST    DirectXデバイスの存在フラグ(0=存在/-1=ロスト)
	SYSREQ_MAXEMITTER エミッター最大数
	SYSREQ_THROUGHFLAG　X方向のボーダー処理フラグ(0=通常/1=スルー)
	SYSREQ_OBAQMATBUF OBAQ用マテリアルバッファ数
	SYSREQ_2DFILTER2   2D直接描画時のテクスチャ補間モード
	SYSREQ_FPUPRESERVE FPU演算精度設定オプション(0=単精度/1=変更なし)
	SYSREQ_DSSOFTWARE  ソフトウェアサウンドバッファの使用(1=ON,0=OFF)
	SYSREQ_DSGLOBAL    グローバルサウンドフォーカス(1=ON,0=OFF)
	SYSREQ_DSBUFSEC    oggストリーム再生バッファのサイズ(秒数)
^p
%href
hgini
hggetreq
%sample
	;	透明抜き色をRGB=($00,$ff,$ff)とする
	;	(texload命令の直前に使用可能)
	hgsetreq SYSREQ_COLORKEY, $00ffff


%index
hggetreq
システムリクエスト取得
%group
拡張画面制御命令
%prm
val,type
val  : 結果が代入される変数名
type : 設定タイプ(SYSREQ_???)
%inst
HGIMG3のシステム設定値を取り出してvalで指定した変数に代入します。
type値で指定できるのは以下のシンボルです。
^p
	シンボル名        内容
-----------------------------------------------
	SYSREQ_MAXMODEL   モデル最大数
	SYSREQ_MAXOBJ     オブジェクト最大数
	SYSREQ_MAXTEX     テクスチャ最大数
	SYSREQ_DXMODE     フルスクリーンモードスイッチ
	SYSREQ_DXHWND     ウィンドウハンドル
	SYSREQ_DXWIDTH    フルスクリーンモード時の横サイズ
	SYSREQ_DXHEIGHT   フルスクリーンモード時の縦サイズ
	SYSREQ_COLORKEY   テクスチャ登録時の透明色コード
	SYSREQ_MAXEVENT   イベント最大数
	SYSREQ_RESULT     エラー発生時の原因コード
	SYSREQ_RESVMODE   ステータスコード
	SYSREQ_PTRD3D     DIRECT3D8のCOMポインタ
	SYSREQ_PTRD3DDEV  DIRECT3DDEVICE8のCOMポインタ
^p
%href
hgini
hgsetreq


%index
hgrect
矩形の直接描画
%group
拡張画面制御命令
%prm
p1,p2,p3,p4,p5,p6
p1=0〜(0)  : 矩形の中心X座標
p2=0〜(0)  : 矩形の中心Y座標
p3=0〜(0.0): 回転角度(単位はラジアン)
p4=0〜(?)  : Xサイズ
p5=0〜(?)  : Yサイズ
%inst
(p1,p2)で指定した座標を中心として、(p4,p5)で指定したサイズの矩形(長方形)を現在設定されている色で描画します。
p3で回転角度を実数で指定することができます。
角度の単位は、ラジアン(0から始まって、2πで一周)となります。
grect命令は、gmodeで設定されたコピーモードの指定が反映されます。
^
gmodeが0,1の場合は、通常の塗りつぶし。
gmodeが3の場合は、指定されたレートで半透明になります。
gmodeが5,6の場合は、それぞれ色加算、色減算処理となります。
また、(p4,p5)のサイズ指定を省略した場合には、gmode命令で設定されているコピーサイズが使用されます。
^
hgrect命令は、標準命令のgrect命令と同じ動作をHGIMG3の画面に対して行ないます。
この命令は、直接描画命令です。命令の実行とともに描画が実行されます。
必ず、hgdrawとhgsyncの間に直接描画を行なう必要があります。

%href
hgline
hgrotate



%index
hgrotate
矩形画像の直接描画
%group
拡張画面制御命令
%prm
p1,p2,p3,p4,p5,p6
p1=0〜(0)  : テクスチャID
p2=0〜(0)  : コピー元の左上X座標
p3=0〜(0)  : コピー元の左上Y座標
p4=0〜(0.0): 回転角度(単位はラジアン)
p5=0〜(?)  : Xサイズ
p6=0〜(?)  : Yサイズ
%inst
hgrotate命令は、指定された矩形範囲に回転を含めたテクスチャ描画処理を行ないます。
p1で、コピー元のテクスチャIDを指定、(p2,p3)でコピーされる元の画像にあたる座標を指定します。(gcopy命令と同様です)
コピー先は、現在の描画先に指定されているウィンドウIDで、pos命令で設定された場所を中心とした座標にコピーを行ないます。
その際に、p3で回転角度を実数で指定することができます。
角度の単位は、ラジアン(0から始まって、2πで一周)となります。
(p5,p6)で、コピーされた後のX,Yサイズを指定します。
また、コピー元のX,Yサイズはgmode命令で設定されたデフォルトのコピーサイズが使用されます。
つまり、gmode命令で指定されたサイズよりも大きなサイズを(p5,p6)で指定した場合には、拡大されることになります。
(p5,p6)を省略した場合には、コピー元と同じサイズ、つまり等倍でコピーが行なわれます。
^
hgrotate命令は、gmodeで設定されたコピーモードの指定が反映されます。
(詳しくはgmode命令のリファレンスを参照)
^
hgrotate命令は、標準命令のgrotate命令と同じ動作をHGIMG3の画面に対して行ないます。
この命令は、直接描画命令です。命令の実行とともに描画が実行されます。
必ず、hgdrawとhgsyncの間に直接描画を行なう必要があります。

%href
hgline
hgrect


%index
hgline
直線の直接描画
%group
拡張画面制御命令
%prm
p1,p2,p3,p4
p1=0〜(0)  : 開始X座標
p2=0〜(0)  : 開始Y座標
p3=0〜(?)  : 終了X座標
p4=0〜(?)  : 終了Y座標
%inst
(p1,p2)で指定した座標から(p3,p4)で指定した座標を結ぶ線を現在設定されている色で描画します。
hgline命令は、gmodeで設定されたコピーモードの指定が反映されます。
^
gmodeが0,1の場合は、通常の塗りつぶし。
gmodeが3の場合は、指定されたレートで半透明になります。
gmodeが5,6の場合は、それぞれ色加算、色減算処理となります。
^
hgline命令は、標準命令のline命令と同じ動作をHGIMG3の画面に対して行ないます。
この命令は、直接描画命令です。命令の実行とともに描画が実行されます。
必ず、hgdrawとhgsyncの間に直接描画を行なう必要があります。

%href
hgrotate
hgrect


%index
settex
テクスチャを登録
%group
拡張画面制御命令
%prm
x,y,sw,mode
(x,y)    : テクスチャ登録サイズ
sw(0)    : テクスチャ登録スイッチ(0=通常/1=Y反転)
mode(-1) : 更新モード指定(-1=新規)
%inst
現在選択されているウィンドウIDの内容をテクスチャとして登録します。
テクスチャの登録に成功すると、システム変数statにテクスチャIDが代入されます。
失敗した場合はシステム変数statがマイナス値になります。
^
swの値が1の場合は、イメージの上下を反転してテクスチャ登録を行ないます。
mode値が-1または省略された場合は、通常の登録処理が行なわれ、mode値にすでに登録されたテクスチャIDを指定すると、同じIDのテクスチャを現在のウィンドウ内容で更新します。
^
通常は、texload命令、texload2命令によってファイルからテクスチャを読み込むようにしてください。
%href
texload
texload2


%index
texload
テクスチャをファイルから登録
%group
拡張画面制御命令
%prm
"filename"
"filename" : 登録するテクスチャの画像ファイル
%inst
画像ファイル"filename"の内容をテクスチャとして登録します。
画像は、picload命令で使用可能なフォーマットと同じもの(BMP,JPG,GIF)が利用可能です。
テクスチャの登録が終了すると、システム変数statにテクスチャIDが代入されます。
失敗した場合はエラーダイアログが表示されます。
^
texload命令は、2の乗数(2,4,8,16…)サイズでないテクスチャは自動的に適正なサイズに補正します。
ビデオカードがテクスチャの色モードやサイズに対応していない場合には、ファイルが存在する場合でもエラーになることがあるので注意してください。
また、アルファチャンネルを含むテクスチャを読み込む場合は、texload2命令を使用してください。
^
この命令は、hgimg3.as内のモジュールとして実装されています。
また、画像読み込み時に、HSPのウィンドウID3を内部で使用しています。
%href
settex
texload2


%index
texload2
テクスチャをファイルから登録
%group
拡張画面制御命令
%prm
"filename",sx,sy
"filename" : 登録するテクスチャの画像ファイル
sx,sy      : 基準となるテクスチャサイズ
%inst
画像ファイル"filename"の内容をテクスチャとして登録します。
texload命令と同じ動作を行ないますが、DirectXのAPIを使用して読み込みが行なわれます。
使用可能なフォーマットは、BMP,JPG,GIF,PNG,DDS,TGA,TIFFなどです。
テクスチャの登録が終了すると、システム変数statにテクスチャIDが代入されます。
失敗した場合はシステム変数statにマイナス値が代入されます。
^
DDSやTGAファイルなどアルファチャンネルを含む画像データは、texload2命令で読み込むことで有効になります。
ただし、texload2命令はビデオカードのスペックに合わせて画像サイズを最適なサイズに拡大縮小を行なうことがあります。
必要に応じてtexload、texload2命令を使い分けるようにしてください。
^
sx,syでテクスチャサイズを強制的に指定することが可能です。
通常は、指定を省略していて問題ありません。


%href
settex
texload



%index
loadtoon
トゥーンテクスチャをファイルから登録
%group
拡張画面制御命令
%prm
p1,"filename"
p1         : モデルID
"filename" : 登録するテクスチャの画像ファイル
%inst
p1で指定したモデルに対してトゥーンシェーディングのモードを設定し、色の情報を持つテクスチャファイルを読み込みます。
^
あらかじめ、addxfile命令によりX形式の3Dモデルが読み込まれている必要があります。
トゥーンシェーディングを設定する場合には、あらかじめトゥーンテクスチャと呼ばれる色情報を持った画像データを作成しておかなければなりません。
(トゥーンテクスチャは、maketoon命令によって作成することが可能です。)
^
この命令は、hgimg3.as内のモジュールとして実装されています。
また、HSPのウィンドウID3を内部で使用しています。

%href
maketoon



%index
maketoon
トゥーンテクスチャを作成
%group
拡張画面制御命令
%prm
p1,p2
p1    : モデルID
p2(0) : 作成モード
%inst
p1で指定したモデルの情報をもとにトゥーンテクスチャの作成を行ないます。
p2で、作成モードを指定することができます。
^p
作成モード  内容
--------------------------------------------------------------
  1         作成されたトゥーンテクスチャをモデルに適用する
  2         作成されたトゥーンテクスチャをファイルにセーブする
^p
maketoon命令は、モデルの持つマテリアル色を抽出し、標準的なトゥーンテクスチャを作成します。
作成モードに2を指定した場合は、「toon.bmp」という名前でトゥーンテクスチャを保存します。
保存されたトゥーンテクスチャは、loadtoon命令で読み込んで使用することが可能なほか、画像を編集することにより、より細かいトゥーンシェーディングの表現を行なうことが可能になります。
^
この命令は、hgimg3.as内のモジュールとして実装されています。
また、HSPのウィンドウID3を内部で使用しています。

%href
loadtoon


%index
setfont
オリジナルフォント定義
%group
拡張画面制御命令
%prm
cx,cy,px,sw
( cx,cy ) : フォント1つあたりのXYサイズ
  px      : 1文字表示ごとに右に移動するドット数
  sw      : 0=透明色抜きなし / 1=透明色抜きあり

%inst
fprt命令で表示するためのフォントを設定します。
使用するテクスチャはこの直後に読み込まれたものになります。
%href
texload
fprt
%sample
	;	テクスチャフォント表示の準備
	;
	setfont 16,16,12,1	; font Tex select(cx,cy,px,mode)
	texload "fontchr.bmp"	; フォントテクスチャの登録


%index
fprt
定義フォント文字列表示
%group
拡張画面制御命令
%prm
"mes",x,y
"mes"  : 表示する文字列
( x,y ) : 表示する座標

%inst
"mes"の内容を指定されたフォントで画面に表示します。
必ずhgdraw命令と、hgsync命令の間に使用してください。
(x,y)の指定は画面の左上が(0,0)になります。
%href
setfont
hgdraw
hgsync
%sample
	;	描画メイン
	;
	hgdraw				; 描画処理
	getsync t1,0			; 前回からの負荷を取得
	fprt "HGIMG Plugin test",8,108
	fprt "T:"+t1,8,124
	hgsync 10			; 処理落ちしてなければ描画


%index
falpha
フォント文字α値を設定
%group
拡張画面制御命令
%prm
val
val : フォント表示に使用するα合成値
%inst
fprt命令によるフォント表示で使用されるα合成値を指定します。
以降すべてのfprt命令に適用されます。
α値についての詳細は以下の通りです。
^p
	α合成値 = 0〜255    : 背景とα合成(blend)を行なう
	α合成値 = 256〜511  : 合成を行なわない(通常時)
	α合成値 = 512〜767  : 背景と色加算(modulate)を行なう
	α合成値 = 768〜1023 : 背景と色減算(substract)を行なう
^p
%href


%index
setsizef
登録モデルの幅、高さを指定
%group
拡張画面制御命令
%prm
fsx,fsy
(fsx,fsy) : X,Yサイズ(実数値)
%inst
モデル登録の際に使用されるサイズ値を指定します。
%href
addspr
addplate
addsplate
addbox


%index
clscolor
背景色設定
%group
拡張画面制御命令
%prm
color
color : カラーコード
%inst
単色で塗りつぶす背景色を設定します。
colorに-1を指定した場合は、背景クリアを行ないません。
colorはRGBコード($rrggbb)となります。
%href
clstex
clsblur


%index
clstex
背景テクスチャ設定
%group
拡張画面制御命令
%prm
id
id : テクスチャID
%inst
背景となるテクスチャのIDを指定します。
idに-1を指定した場合は、背景にテクスチャを使わなくなります。
%href
clscolor
clsblur
texload


%index
clsblur
ブラー設定
%group
拡張画面制御命令
%prm
val
val : 強度(0〜255)
%inst
擬似ブラー効果を設定します。
valで設定した強度の値が小さいほど、画面に前のフレームが残像として残ります。
%href
clscolor
clstex


%index
texmake
メッセージテクスチャの作成
%group
拡張画面制御命令
%prm
p1,p2,p3
p1(256) : テクスチャXサイズ
p2(256) : テクスチャYサイズ
p3(0)   : 作成モード
%inst
メッセージ描画用テクスチャの作成を行ないます。
このテクスチャは、texmes命令のメッセージ描画先として使用することができます。
^
テクスチャの登録が終了すると、システム変数statにテクスチャIDが代入されます。
失敗した場合はシステム変数statにマイナス値が代入されます。
^
作成モードが0の場合は、16bitテクスチャ(A4R4G4B4)が作成されます。
0以外の時は、32bitテクスチャ(A8R8G8B8)となります。
デフォルトでは、16bitテクスチャが作成されます。
メッセージを扱う場合は、16bitテクスチャの方が速度的にも互換性の上でも有利になります。
より高精度なメッセージ描画を行なう場合には、32bitテクスチャを指定してください。

%href
texcls
texmes


%index
texcls
メッセージテクスチャのクリア
%group
拡張画面制御命令
%prm
p1,p2
p1    : テクスチャID
p2(0) : カラーコード
%inst
メッセージ描画用テクスチャの内容をクリア(消去)します。
p2で、単色で塗りつぶすための背景色を設定します。
p2はRGBコード($rrggbb)となります。

%href
texmake
texmes


%index
texmes
メッセージテクスチャへの描画
%group
拡張画面制御命令
%prm
"message",p1,p2,p3
"message" : メッセージ文字列
p1        : テクスチャID
p2,p3     : 描画先座標
%inst
メッセージ描画用テクスチャにメッセージを書き込みます。
"message"に描画したい文字列を指定することで、p1のテクスチャに書き込みを行ないます。
あらかじめ、texmake命令によりメッセージ描画用テクスチャを作成しておく必要があるので注意してください。
^
texmes命令は、アンチエイリアスを行なった文字をテクスチャに対して書き込みます。
描画される文字フォントや色は、font命令、color命令で指定されているものが使われます。
texmes命令は、テクスチャに対して文字列を描画するだけなので、実際に文字列を表示するためには、テクスチャをhgrotate命令やスプライトなどで表示する必要があります。

%href
color
font
texmake
texcls
texopt


%index
texdel
テクスチャの削除
%group
拡張画面制御命令
%prm
p1
p1    : テクスチャID
%inst
p1で指定されたテクスチャを削除します。
削除されたテクスチャは、VRAM上から破棄され新しく入れ替えることができるようになります。

%href
texmake
texload
texload2


%index
setuv
登録テクスチャUV座標を指定
%group
拡張画面制御命令
%prm
tx0,ty0,tx1,ty1
(tx0,ty0) : テクスチャの左上座標
(tx1,ty1) : テクスチャの右下座標
%inst
モデル登録の際に使用されるUV値を指定します。
%href
addspr
addplate
addbox


%index
addspr
2Dスプライトモデルを作成
%group
拡張画面制御命令
%prm
var,mode,x1,y1,x2,y2,texid
var     : 作成されたモデルIDが代入される変数名
mode    : 0=透明色抜きなし / 1=透明色抜きあり
(x1,y1) : テクスチャの左上座標
(x2,y2) : テクスチャの右下座標
texid   : テクスチャのID
%inst
2Dスプライトモデルを作成します。
正常にモデルが作成されると、varで指定した変数にモデルIDが代入されます。
texidが指定されている場合は、それをテクスチャIDとして参照します。
texidが省略された場合は、次に登録されるテクスチャが参照されます。
(x1,y1)-(x2,y2)で参照されるテクスチャの座標を設定することができます。
(x1,y1)-(x2,y2)の指定を省略した場合には、setuvで指定された値が適用されます。

%href
setuv


%index
regobj
オブジェクトの登録
%group
拡張画面制御命令
%prm
var,ModelID,mode,EventID
var      : 作成されたオブジェクトIDが代入される変数名
modelID  : モデルID
mode     : モード値
EventID  : イベントID
%inst
指定されたモデルを表示するためのオブジェクトを作成します。
成功すると作成されたオブジェクトIDが変数に代入されます。
何らかの理由で作成に失敗した場合は、-1が代入されます。
^
モードは以下の中から選択します。
^p
	ラベル             |        内容
	--------------------------------------------------------------
	OBJ_HIDE             非表示(画面から消す)
	OBJ_TREE             木属性(Y軸のみ正面を向く)
	OBJ_XFRONT           正面属性(常に画面に正面を向く)
	OBJ_MOVE             XYZ移動量を有効にする
	OBJ_FLIP             ボーダー領域で反転する
	OBJ_BORDER           ボーダー領域を有効にする
	OBJ_LATE             常に後から描かれる(半透明オブジェクト用)
	OBJ_FIRST            常に最初に描かれる
	OBJ_SORT             自動的に奥から描かれる(3Dオブジェクト用)
	OBJ_LOOKAT           特定オブジェクトの方向を常に向く
	OBJ_LAND             Y座標を常に一定に保つ
	OBJ_GROUND           地面として認識される
	OBJ_STAND            地面の上に配置する
	OBJ_GRAVITY          重力計算を有効にする
	OBJ_STATIC           障害物として認識される
	OBJ_BOUND            地面で反発する(メッシュマップコリジョン用)
	OBJ_ALIEN            ターゲットに向ける(メッシュマップコリジョン用)
	OBJ_WALKCLIP         移動の制限を受ける(メッシュマップコリジョン用)
	OBJ_EMITTER          エミッター発生オブジェクトになる
^p
複数の項目を同時に選択する場合は、「OBJ_LATE|OBJ_MOVE」のように「|」で区切って指定してください。何も指定しない場合は、0にするか省略して構いません。
オブジェクトのモードは、あらかじめデフォルトのモードがモデルの種類に応じて設定されているので、通常は特に設定する必要はありません。
OBJ_2Dのモードは、自動的に設定されるもので、途中で変更しないようにしてください。
EventIDを指定すると、該当するイベントがオブジェクトに設定されます。
EventIDを省略するか、マイナス値の場合はイベントを設定しません。
%href
delobj


%index
addplate
板(PLATE)モデルを作成
%group
拡張画面制御命令
%prm
var,mode,sx,sy,x1,y1,x2,y2,texid
var     : 作成されたモデルIDが代入される変数名
mode    : 0=透明色抜きなし / 1=透明色抜きあり
(sx,sy) : X,Yサイズ
(x1,y1) : テクスチャの左上座標
(x2,y2) : テクスチャの右下座標
texid   : テクスチャID
%inst
3Dの板(PLATE)モデルを作成します。
正常にモデルが作成されると、varで指定した変数にモデルIDが代入されます。
PLATEモデルは、単色またはテクスチャで描画され3D上に配置される四角形です。
setcolor命令で色が設定されている時は、単色の板になります。
texidが指定されている場合は、それをテクスチャIDとして参照します。
texidが省略された場合は、次に登録されるテクスチャが参照されます。
(sx,sy)で板のX,Yサイズを設定します。省略された場合は、setsizefで指定された値が適用されます。
(x1,y1)-(x2,y2)で参照されるテクスチャの座標を設定することができます。
(x1,y1)-(x2,y2)の指定を省略した場合には、setuvで指定された値が適用されます。
PLATEモデルは、光源計算を行ないません。
光源計算が必要な場合には、addsplate命令によりSPLATEモデルを作成してください。

%href
addsplate
setcolor
setsizef
setuv


%index
addsplate
板(SPLATE)モデルを作成
%group
拡張画面制御命令
%prm
var,mode,sx,sy,texid
var     : 作成されたモデルIDが代入される変数名
mode    : 0=透明色抜きなし / 1=透明色抜きあり
(sx,sy) : X,Yサイズ
texid   : テクスチャID
%inst
3Dの光源計算付き板(SPLATE)モデルを作成します。
正常にモデルが作成されると、varで指定した変数にモデルIDが代入されます。
SPLATEモデルは、単色またはテクスチャで描画され3D上に配置される四角形です。
setcolor命令で色が設定されている時は、単色の板になります。
texidが指定されている場合は、それをテクスチャIDとして参照します。
texidが省略された場合は、次に登録されるテクスチャが参照されます。
(sx,sy)で板のX,Yサイズを設定します。省略された場合は、setsizefで指定された値が適用されます。
addplate命令とほぼ同じですが、光源計算を行なうことと、テクスチャUV座標の指定ができない点が異なっています。

%href
addplate
setcolor
setsizef


%index
setcolor
単色ポリゴン作成設定
%group
拡張画面制御命令
%prm
r,g,b
r,g,b : RGB色情報(それぞれ0〜255)
%inst
モデルの作成を行なう際のポリゴン色を設定します。
addplate,addbox命令などで単色ポリゴンを作成する場合に使用します。
%href
addplate
addbox


%index
addbox
箱(BOX)モデルを作成
%group
拡張画面制御命令
%prm
var,sx,sy,texid
var     : 作成されたモデルIDが代入される変数名
(sx,sy) : X,Yサイズ
texid   : テクスチャID
%inst
箱(BOX)モデルを作成します。
正常にモデルが作成されると、varで指定した変数にモデルIDが代入されます。
BOXモデルは、単色またはテクスチャで描画され3D上に配置される立方体です。
texidが指定されている場合は、それをテクスチャIDとして参照します。
setcolor命令で色が設定されている時は、単色の箱になります。
setuv命令によってUVが指定されており、texidが省略された場合は、
次に登録されるテクスチャが参照されます。(ただしUVの指定は反映されません)
(sx,sy)で板のX,Yサイズを設定します。省略された場合は、setsizefで指定された値が適用されます。

%href
setsizef
setcolor
setuv


%index
addmesh
板(MESH)モデルを作成
%group
拡張画面制御命令
%prm
var,divx,divy,mode,sx,sy,texid
var     : 作成されたモデルIDが代入される変数名
divx    : メッシュの分割数X
divy    : メッシュの分割数Y
mode    : 0=透明色抜きなし / 1=透明色抜きあり / 16=繰り返し
(sx,sy) : X,Yサイズ
texid   : テクスチャID
%inst
分割された3Dポリゴンを使った板(MESH)を作成します。
正常にモデルが作成されると、varで指定した変数にモデルIDが代入されます。
BOXモデルは、テクスチャで描画され3D上に配置される分割メッシュです。
texidが指定されている場合は、それをテクスチャIDとして参照します。
setuv命令によってUVが指定されており、texidが省略された場合は、次に登録されるテクスチャが参照されます。(ただしUVの指定は反映されません)
(sx,sy)で板のX,Yサイズを設定します。省略された場合は、setsizefで指定された値が適用されます。
^
基本的にaddplateで作られる板と見た目は同じですが、divsizex × divsizeyのポリゴンに分割されています。
modeが16の場合は、指定されたテクスチャを1ポリゴンごとに割り付ける繰り返しモードとなります。
%href
setsizef
setuv
meshmap
objwalk


%index
addeprim
エフェクト(EPRIM)モデルを作成
%group
拡張画面制御命令
%prm
var,mode,option
var     : 作成されたモデルIDが代入される変数名
mode    : プリミティブモード
option  : オプション値
%inst
エフェクト(EPRIM)モデルを作成します。
正常にモデルが作成されると、varで指定した変数にモデルIDが代入されます。
エフェクトモデルは、効果生成用の特殊な形状を持ったモデルです。
エフェクトモデルは頂点ごとに色が設定され、光源計算を行なわない特別なモデルで、形状をseteprim命令でカスタマイズすることができます。
modeパラメーターにより、以下の形状を選択することができます。
^p
	ラベル             |        内容
	--------------------------------------------------------------
	EPRIM_CIRCLE         円
	EPRIM_CIRCLE2        ギザギザの円
	EPRIM_SQUARE         角の丸い四角形
	EPRIM_FAN            任意の頂点数を持つ多角形
^p
optionパラメーターにより、モデル作成をコントロールすることができます。
option値が0か省略された場合は、通常通りのモデルが作成されます。
option値が1の場合は、モデル本体の生成は行なわず、周囲のりん光(SPREAD)のみ生成します。
option値が2の場合は、りん光(SPREAD)の生成は行なわず、モデル本体のみ生成します。
%href
seteprim


%index
seteprim
エフェクト(EPRIM)モデルを設定
%group
拡張画面制御命令
%prm
Model,Param,Value
Model   : モデルID
Param   : パラメーターID
Value   : パラメーターに設定する値
%inst
エフェクト(EPRIM)モデルの生成パラメーターを設定します。
Modelに、addeprim命令によって作成されたモデルIDを指定します。
Paramに設定するパラメーターIDを、Valueに設定する値を実数か整数により指定します。
Paramで指定するパラメーターIDは、addeprim命令で作成されたモデル形状により異なります。
パラメーターIDは、0から8までが実数値による設定が可能です。
パラメーターIDの16から19までは、色コードの設定となりα値、RGB値を持つ32bitの整数値を設定することができます。
seteprim命令により、生成パラメーターを変更した後は、表示を行なう際に頂点座標の再構築が行なわれます。
処理の負荷を軽減するためには、毎フレームごとにseteprim命令を実行するのは避けて、
必要な場合にのみパラメーター変更を行なうようにしてください。
%href
addeprim


%index
setefx
efxグループ情報を設定
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 設定する値 (デフォルト=0)

%inst
オブジェクトの持つパラメーターを設定します。
efxグループ(特殊効果設定)に(x,y,z)で指定された値を設定します。
(x,y,z)には、実数または整数値を指定することができます。
^
オブジェクトに対し、xの値は次のように認識されます。y,zの値は未使用です。
^p
0 〜255 : 背景とα合成(blend)を行なう
256 〜511 : 合成を行なわない(通常時)（現在、ブレンド濃度は関係ありません）
512 〜767 : 背景と色加算(modulate)を行なう
768 〜1023 : 背景とα値減算(substract)を行なう
1024 〜1279 : 背景と色減算(substract2)を行なう（現在、ブレンド濃度は無効になります）
1280 〜1535 : 合成を行なわない(Z無視)（現在、ブレンド濃度は関係ありません）
^p
カメラに対し、このパラメータを指定する場合、x,y,zを次のように認識します。
^p
x : 視野角度。初期値は45度(π/4)が指定されています。
y : NearZ値。カメラからこの距離より手前のポリゴンは表示されません。
z : FarZ値。カメラからこの距離より奥のポリゴンは表示されません。
^p


%href
setpos
setang
setangr
setscale
setdir
setwork


%index
addefx
efxグループ情報を加算
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 加算する値 (デフォルト=0)

%inst
オブジェクトの持つパラメーターを設定します。
efxグループ(特殊効果設定)に(x,y,z)で指定された値を加算します。
(x,y,z)には、実数または整数値を指定することができます。

%href
addpos
addang
addangr
addscale
adddir
addwork


%index
getang
angグループ情報を取得
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 取得する変数

%inst
オブジェクトの持つパラメーターを取得します。
angグループ(表示角度)の内容が(x,y,z)で指定された変数に代入されます。
(x,y,z)は、実数型の変数として設定されます。
命令の最後に「i」を付加することで、整数値として値を取得することができます。

%href
getangi
getpos
getangr
getscale
getdir
getefx
getwork


%index
getangr
angグループ情報を取得
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 取得する変数

%inst
オブジェクトの持つパラメーターを取得します。
angグループ(表示角度)の内容が(x,y,z)で指定された変数に代入されます。
(x,y,z)は、整数型の変数として設定されます。
角度の単位は整数で0〜255で一周する値を使用します。

%href
getpos
getang
getscale
getdir
getefx
getwork


%index
getefx
efxグループ情報を取得
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 取得する変数

%inst
オブジェクトの持つパラメーターを取得します。
efxグループ(特殊効果設定)の内容が(x,y,z)で指定された変数に代入されます。
(x,y,z)は、実数型の変数として設定されます。
命令の最後に「i」を付加することで、整数値として値を取得することができます。

%href
getefxi
getpos
getang
getangr
getscale
getdir
getwork


%index
getangi
angグループ情報を整数で取得
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 取得する変数

%inst
オブジェクトの持つパラメーターを取得します。
angグループ(表示角度)の内容が(x,y,z)で指定された変数に代入されます。
(x,y,z)は、整数型の変数として設定されます。

%href
getang
getposi
getangri
getscalei
getdiri
getefxi
getworki


%index
getefxi
efxグループ情報を整数で取得
%group
拡張画面制御命令
%prm
id,x,y,z
id      : オブジェクトID
(x,y,z) : 取得する変数

%inst
オブジェクトの持つパラメーターを取得します。
efxグループ(特殊効果設定)の内容が(x,y,z)で指定された変数に代入されます。
(x,y,z)は、整数型の変数として設定されます。

%href
getefx
getposi
getangi
getangri
getscalei
getdiri
getworki


%index
selefx
オブジェクト特殊効果をMOC情報に設定
%group
拡張画面制御命令
%prm
id
id     : オブジェクトID
%inst
MOC設定命令の対象となるMOCグループをefx(効果)に設定します
idは、オブジェクトIDとなります。
%href
selmoc
selpos
selang
selscale
selcam
selcpos
selcang
selcint


%index
objset1
MOC情報を設定
%group
拡張画面制御命令
%prm
ofs,x
ofs : MOCのオフセット番号
x   : 設定する値

%inst
MOC情報を設定します。
%href
objset1
objadd1
objset1r
objsetf1
objaddf1


%index
objsetf1
MOC情報を設定
%group
拡張画面制御命令
%prm
ofs,fx
ofs : MOCのオフセット番号
fx  : 設定する値(実数値)

%inst
MOC情報を設定します。
%href
objset1
objadd1
objset1r
objsetf1
objaddf1


%index
objadd1
MOC情報を加算
%group
拡張画面制御命令
%prm
ofs,x
ofs : MOCのオフセット番号
x   : 加算する値

%inst
MOC情報に設定されている値にxを加算します。
%href
objset1
objadd1r
objset1r
objsetf1
objaddf1


%index
objadd1r
MOC情報を加算
%group
拡張画面制御命令
%prm
ofs,x
ofs : MOCのオフセット番号
x   : 加算する値(整数角度値)

%inst
MOC情報に設定されている値にxを加算します。
ただし整数値(256で１回転)をラジアン単位に変換したパラメーターを加算します。
角度を指定するパラメーター以外では正常な値にならないので注意してください。
%href
objset1
objadd1
objset1r
objsetf1
objaddf1


%index
objaddf1
MOC情報を加算
%group
拡張画面制御命令
%prm
ofs,fx
ofs : MOCのオフセット番号
fx  : 加算する値(実数値)

%inst
MOC情報に設定されている値にfxを加算します。
%href
objset1
objadd1
objset1r
objsetf1
objaddf1


%index
objset1r
MOC情報を設定
%group
拡張画面制御命令
%prm
ofs,x
ofs : MOCのオフセット番号
x   : 設定する値

%inst
MOC情報に角度情報を設定します。
整数値(256で１回転)をラジアン単位に変換してパラメーターを書き込みます。
角度を指定するパラメーター以外では正常な値にならないので注意してください。
%href
objset1
objadd1
objset1r
objsetf1
objaddf1


%index
objset2
MOC情報を設定
%group
拡張画面制御命令
%prm
ofs,x,y
ofs : MOCのオフセット番号
x   : 設定する値
y   : 設定する値2

%inst
MOC情報を設定します。
ofsで指定したオフセット番号から２つのパラメータが対象になります。
%href
objset2
objadd2
objset2r
objsetf2
objaddf2


%index
objsetf2
MOC情報を設定
%group
拡張画面制御命令
%prm
ofs,fx,fy
ofs : MOCのオフセット番号
fx  : 設定する値(実数値)
fy  : 設定する値2(実数値)

%inst
MOC情報を設定します。
ofsで指定したオフセット番号から２つのパラメータが対象になります。
%href
objset2
objadd2
objset2r
objsetf2
objaddf2


%index
objadd2
MOC情報を加算
%group
拡張画面制御命令
%prm
ofs,x,y
ofs : MOCのオフセット番号
x   : 加算する値
y   : 加算する値2

%inst
MOC情報に設定されている値にx,yを加算します。
ofsで指定したオフセット番号から２つのパラメータが対象になります。
%href
objset2
objadd2r
objset2r
objsetf2
objaddf2


%index
objaddf2
MOC情報を加算
%group
拡張画面制御命令
%prm
ofs,fx,fy
ofs : MOCのオフセット番号
fx  : 加算する値(実数値)
fy  : 加算する値2(実数値)

%inst
MOC情報に設定されている値にfx,fyを加算します。
ofsで指定したオフセット番号から２つのパラメータが対象になります。
%href
objset2
objadd2
objset2r
objsetf2
objaddf2


%index
objadd2r
MOC情報を加算
%group
拡張画面制御命令
%prm
ofs,fx,fy
ofs : MOCのオフセット番号
fx  : 加算する値(整数角度値)
fy  : 加算する値2(整数角度値)
%inst
MOC情報に設定されている値にfx,fyを加算します。
ただし整数値(256で１回転)をラジアン単位に変換したパラメーターを加算します。
角度を指定するパラメーター以外では正常な値にならないので注意してください。
%href
objset2
objadd2
objset2r
objsetf2
objaddf2


%index
objset2r
MOC情報を設定
%group
拡張画面制御命令
%prm
ofs,x,y
ofs : MOCのオフセット番号
x   : 設定する値
y   : 設定する値2

%inst
MOC情報に角度情報を設定します。
ofsで指定したオフセット番号から２つのパラメータが対象になります。
整数値(256で１回転)をラジアン単位に変換してパラメーターを書き込みます。
角度を指定するパラメーター以外では正常な値にならないので注意してください。
%href
objset2
objadd2
objset2r
objsetf2
objaddf2


%index
dxfload
DXF形式ファイルを読み込み
%group
拡張画面制御命令
%prm
"filename",p1
"filename" : 読み込むモデルファイル(DXF形式)
p1(0)      : 表面の色指定(オプション)
%inst
DXF形式のファイルをモデルデータとして読み込みます。
システム変数statに読み込まれた先のモデルIDが返されます。
p1で読み込まれたポリゴンの表面色を指定することができます。表面色は、RGBを24bit整数にパックした形式($rrggbb)で指定する必要があります。
p1を省略または0に指定した場合は、白色(0xffffff)が選択されます。
DXF形式は、CADやほとんどの3Dツールがサポートしている基本的な3D形状フォーマットです。テクスチャ情報など高度な情報は反映されませんが、形状をやり取りすることが可能です。
dxfload命令では、DXF形式の3DFACEというタイプにのみ対応しています。
一部のファイルとは互換がない可能性があります。
この命令は、hgimg3.asモジュール内で定義されています。
%href
adddxf
%sample
	dxfload "ball.dxf"		; dxfファイル読み込み
	m_model=stat
	regobj obj, m_model		; オブジェクトの登録


%index
adddxf
DXF形式データの登録
%group
拡張画面制御命令
%prm
var1,var2,color
var1     : 作成されたモデルIDが代入される変数名
var2     : DXF形式のデータが代入されているバッファ変数名
color(0) : 表面の色指定(オプション)
%inst
DXF形式のモデルを登録します。モデルデータは、あらかじめvar2で指定された変数バッファに読み込まれている必要があります。
正常にモデルが登録されると、varで指定した変数にモデルIDが代入されます。
colorで読み込まれたポリゴンの表面色を指定することができます。表面色は、RGBを24bit整数にパックした形式($rrggbb)で指定する必要があります。
colorを省略または0に指定した場合は、白色(0xffffff)が選択されます。
この命令は、メモリ上に読み込まれたデータから登録を行なうためのものです。DXF形式のファイルが用意されている場合は、dxfload命令をお使いください。
%href
dxfload


%index
modelscale
モデルのサイズを変更する
%group
拡張画面制御命令
%prm
id,x,y,z
id      : モデルID
(x,y,z) : X,Y,Z倍率(実数値)
%inst
指定したモデル全体を指定したスケール値で拡大縮小します。
これによりモデルそのもののサイズ自体を変えることが可能になります。


%index
event_wait
待ち時間イベントを追加
%group
拡張画面制御命令
%prm
id,p1
id      : イベントID
p1(0)   : 待ち時間(フレーム)
%inst
idで指定しているイベントIDに、待ち時間イベントを追加します。
待ち時間イベントは、p1で指定されたフレーム数だけ次のイベントに進むことを保留します。

%href
newevent
setevent


%index
event_jump
ジャンプイベントを追加
%group
拡張画面制御命令
%prm
id,p1,p2
id      : イベントID
p1(0)   : ジャンプ先のイベント番号
p2(0)   : ジャンプ無視の確率(%)
%inst
idで指定しているイベントIDに、ジャンプイベントを追加します。
ジャンプイベントは、指定されたイベント番号から実行を続けることを指示します。
イベントリストの中でのgoto命令にあたります。
p1で指定するイベント番号は、イベントに追加された順番に0,1,2…と数えたものになります。
p2で、ジャンプ無視の確率(%)を設定することができます。
0または省略された場合は、必ず(無条件)でジャンプを行ないます。
それ以外の場合は、乱数をもとに1〜100%の確率でジャンプを行ない、
ジャンプしなかった場合は次のイベントに進みます。

%href
newevent
setevent


%index
event_prmset
パラメーター設定イベントを追加
%group
拡張画面制御命令
%prm
id,p1,p2
id      : イベントID
p1(0)   : パラメーターID(PRMSET_*)
p2(0)   : 設定される値
%inst
idで指定しているイベントIDに、パラメーター設定イベントを追加します。
パラメーター設定イベントは、p1で指定されたパラメーターIDにp2の値を設定します。
(それまでに設定されていた内容は消去されます)
パラメーターIDには、以下の名前を使用することができます。
^p
パラメーターID   内容
---------------------------------------
PRMSET_MODE      動作モード
PRMSET_FLAG      存在フラグ
PRMSET_SHADE     シェーディングモード
PRMSET_TIMER     タイマー
PRMSET_MYGROUP   コリジョングループ値
PRMSET_COLGROUP  対象グループ値
^p

%href
event_prmon
event_prmoff
newevent
setevent


%index
event_prmon
パラメータービット設定イベントを追加
%group
拡張画面制御命令
%prm
id,p1,p2
id      : イベントID
p1(0)   : パラメーターID(PRMSET_*)
p2(0)   : 設定されるビット
%inst
idで指定しているイベントIDに、パラメータービット設定イベントを追加します。
パラメータービット設定イベントは、p1で指定されたパラメーターIDにp2のビットを設定します。
(それまでに設定されていた内容は保持されたまま、新しい値のビットだけが有効になります)
パラメーターIDの詳細については、event_prmset命令を参照してください。

%href
event_prmset
event_prmoff
newevent
setevent


%index
event_prmoff
パラメータービット消去イベントを追加
%group
拡張画面制御命令
%prm
id,p1,p2
id      : イベントID
p1(0)   : パラメーターID(PRMSET_*)
p2(0)   : 消去されるビット
%inst
idで指定しているイベントIDに、パラメータービット消去イベントを追加します。
パラメータービット消去イベントは、p1で指定されたパラメーターIDから、
p2のビットだけを消去します。
パラメーターIDの詳細については、event_prmset命令を参照してください。

%href
event_prmset
event_prmon
newevent
setevent


%index
event_setpos
posグループ設定イベントを追加
%group
拡張画面制御命令
%prm
id,x1,y1,z1,x2,y2,z2
id         : イベントID
(x1,y1,z1) : 設定される値(下限値)
(x2,y2,z2) : 設定される値(上限値)
%inst
idで指定しているイベントIDに、グループ設定イベントを追加します。
グループ設定イベントは、オブジェクトが持っているパラメーターを設定します。
(x1,y1,z1)と(x2,y2,z2)を指定すると、それぞれの範囲内にある値を乱数で作成します。
(x2,y2,z2)を省略して、(x1,y1,z1)だけを指定した場合はそのまま値が設定されます。

%href
event_setang
event_setangr
event_setscale
event_setdir
event_setefx
event_setwork
newevent
setevent


%index
event_setang
angグループ設定イベントを追加
%group
拡張画面制御命令
%prm
id,x1,y1,z1,x2,y2,z2
id         : イベントID
(x1,y1,z1) : 設定される値(下限値)
(x2,y2,z2) : 設定される値(上限値)
%inst
idで指定しているイベントIDに、グループ設定イベントを追加します。
グループ設定イベントは、オブジェクトが持っているパラメーターを設定します。
(x1,y1,z1)と(x2,y2,z2)を指定すると、それぞれの範囲内にある値を乱数で作成します。
(x2,y2,z2)を省略して、(x1,y1,z1)だけを指定した場合はそのまま値が設定されます。

%href
event_setpos
event_setangr
event_setscale
event_setdir
event_setefx
event_setwork
newevent
setevent


%index
event_setangr
angグループ設定イベントを追加
%group
拡張画面制御命令
%prm
id,x1,y1,z1,x2,y2,z2
id         : イベントID
(x1,y1,z1) : 設定される値(下限値)
(x2,y2,z2) : 設定される値(上限値)
%inst
idで指定しているイベントIDに、グループ設定イベントを追加します。
グループ設定イベントは、オブジェクトが持っているパラメーターを設定します。
(x1,y1,z1)と(x2,y2,z2)を指定すると、それぞれの範囲内にある値を乱数で作成します。
(x2,y2,z2)を省略して、(x1,y1,z1)だけを指定した場合はそのまま値が設定されます。

%href
event_setpos
event_setang
event_setscale
event_setdir
event_setefx
event_setwork
newevent
setevent


%index
event_setscale
scaleグループ設定イベントを追加
%group
拡張画面制御命令
%prm
id,x1,y1,z1,x2,y2,z2
id         : イベントID
(x1,y1,z1) : 設定される値(下限値)
(x2,y2,z2) : 設定される値(上限値)
%inst
idで指定しているイベントIDに、グループ設定イベントを追加します。
グループ設定イベントは、オブジェクトが持っているパラメーターを設定します。
(x1,y1,z1)と(x2,y2,z2)を指定すると、それぞれの範囲内にある値を乱数で作成します。
(x2,y2,z2)を省略して、(x1,y1,z1)だけを指定した場合はそのまま値が設定されます。

%href
event_setpos
event_setang
event_setangr
event_setdir
event_setefx
event_setwork
newevent
setevent


%index
event_setdir
dirグループ設定イベントを追加
%group
拡張画面制御命令
%prm
id,x1,y1,z1,x2,y2,z2
id         : イベントID
(x1,y1,z1) : 設定される値(下限値)
(x2,y2,z2) : 設定される値(上限値)
%inst
idで指定しているイベントIDに、グループ設定イベントを追加します。
グループ設定イベントは、オブジェクトが持っているパラメーターを設定します。
(x1,y1,z1)と(x2,y2,z2)を指定すると、それぞれの範囲内にある値を乱数で作成します。
(x2,y2,z2)を省略して、(x1,y1,z1)だけを指定した場合はそのまま値が設定されます。

%href
event_setpos
event_setang
event_setangr
event_setscale
event_setefx
event_setwork
newevent
setevent


%index
event_setefx
efxグループ設定イベントを追加
%group
拡張画面制御命令
%prm
id,x1,y1,z1,x2,y2,z2
id         : イベントID
(x1,y1,z1) : 設定される値(下限値)
(x2,y2,z2) : 設定される値(上限値)
%inst
idで指定しているイベントIDに、グループ設定イベントを追加します。
グループ設定イベントは、オブジェクトが持っているパラメーターを設定します。
(x1,y1,z1)と(x2,y2,z2)を指定すると、それぞれの範囲内にある値を乱数で作成します。
(x2,y2,z2)を省略して、(x1,y1,z1)だけを指定した場合はそのまま値が設定されます。

%href
event_setpos
event_setang
event_setangr
event_setscale
event_setdir
event_setwork
newevent
setevent


%index
event_setwork
workグループ設定イベントを追加
%group
拡張画面制御命令
%prm
id,x1,y1,z1,x2,y2,z2
id         : イベントID
(x1,y1,z1) : 設定される値(下限値)
(x2,y2,z2) : 設定される値(上限値)
%inst
idで指定しているイベントIDに、グループ設定イベントを追加します。
グループ設定イベントは、オブジェクトが持っているパラメーターを設定します。
(x1,y1,z1)と(x2,y2,z2)を指定すると、それぞれの範囲内にある値を乱数で作成します。
(x2,y2,z2)を省略して、(x1,y1,z1)だけを指定した場合はそのまま値が設定されます。

%href
event_setpos
event_setang
event_setangr
event_setscale
event_setdir
event_setefx
newevent
setevent


%index
event_pos
posグループ変化イベントを追加
%group
拡張画面制御命令
%prm
id,frame,x1,y1,z1,sw
id         : イベントID
frame      : 変化までのフレーム数
(x1,y1,z1) : 設定される値
sw(1)      : 補間オプション
%inst
idで指定しているイベントIDに、グループ変化イベントを追加します。
グループ変化イベントは、オブジェクトが持っているパラメーターの時間による変化を設定します。
frameで指定したフレーム数が経過した時に(x1,y1,z1)の値になります。
swの補間オプションは、以下の値を指定することができます。
^p
	sw = 0 : リニア補間(絶対値)
	sw = 1 : スプライン補間(絶対値)
	sw = 2 : リニア補間(相対値)
	sw = 3 : スプライン補間(相対値)
^p
swを省略した場合には、絶対値スプラインが設定されます。

%href
event_ang
event_angr
event_scale
event_dir
event_efx
event_work
newevent
setevent


%index
event_ang
angグループ変化イベントを追加
%group
拡張画面制御命令
%prm
id,frame,x1,y1,z1,sw
id         : イベントID
frame      : 変化までのフレーム数
(x1,y1,z1) : 設定される値
sw(0)      : 補間オプション
%inst
idで指定しているイベントIDに、グループ変化イベントを追加します。
グループ変化イベントは、オブジェクトが持っているパラメーターの時間による変化を設定します。
frameで指定したフレーム数が経過した時に(x1,y1,z1)の値になります。
swの補間オプションは、以下の値を指定することができます。
^p
	sw = 0 : リニア補間(絶対値)
	sw = 1 : スプライン補間(絶対値)
	sw = 2 : リニア補間(相対値)
	sw = 3 : スプライン補間(相対値)
^p
swを省略した場合には、絶対値リニアが設定されます。

%href
event_pos
event_angr
event_scale
event_dir
event_efx
event_work
newevent
setevent


%index
event_angr
angグループ変化イベントを追加
%group
拡張画面制御命令
%prm
id,frame,x1,y1,z1
id         : イベントID
frame      : 変化までのフレーム数
(x1,y1,z1) : 設定される値
%inst
idで指定しているイベントIDに、グループ変化イベントを追加します。
グループ変化イベントは、オブジェクトが持っているパラメーターの時間による変化を設定します。
frameで指定したフレーム数が経過した時に(x1,y1,z1)の値になります。

%href
event_pos
event_ang
event_scale
event_dir
event_efx
event_work
newevent
setevent


%index
event_scale
scaleグループ変化イベントを追加
%group
拡張画面制御命令
%prm
id,frame,x1,y1,z1,sw
id         : イベントID
frame      : 変化までのフレーム数
(x1,y1,z1) : 設定される値
sw(0)      : 補間オプション
%inst
idで指定しているイベントIDに、グループ変化イベントを追加します。
グループ変化イベントは、オブジェクトが持っているパラメーターの時間による変化を設定します。
frameで指定したフレーム数が経過した時に(x1,y1,z1)の値になります。
swの補間オプションは、以下の値を指定することができます。
^p
	sw = 0 : リニア補間(絶対値)
	sw = 1 : スプライン補間(絶対値)
	sw = 2 : リニア補間(相対値)
	sw = 3 : スプライン補間(相対値)
^p
swを省略した場合には、絶対値リニアが設定されます。

%href
event_pos
event_ang
event_angr
event_dir
event_efx
event_work
newevent
setevent


%index
event_dir
dirグループ変化イベントを追加
%group
拡張画面制御命令
%prm
id,frame,x1,y1,z1,sw
id         : イベントID
frame      : 変化までのフレーム数
(x1,y1,z1) : 設定される値
sw(0)      : 補間オプション
%inst
idで指定しているイベントIDに、グループ変化イベントを追加します。
グループ変化イベントは、オブジェクトが持っているパラメーターの時間による変化を設定します。
frameで指定したフレーム数が経過した時に(x1,y1,z1)の値になります。
swの補間オプションは、以下の値を指定することができます。
^p
	sw = 0 : リニア補間(絶対値)
	sw = 1 : スプライン補間(絶対値)
	sw = 2 : リニア補間(相対値)
	sw = 3 : スプライン補間(相対値)
^p
swを省略した場合には、絶対値リニアが設定されます。

%href
event_pos
event_ang
event_angr
event_scale
event_efx
event_work
newevent
setevent


%index
event_efx
efxグループ変化イベントを追加
%group
拡張画面制御命令
%prm
id,frame,x1,y1,z1,sw
id         : イベントID
frame      : 変化までのフレーム数
(x1,y1,z1) : 設定される値
sw(0)      : 補間オプション
%inst
idで指定しているイベントIDに、グループ変化イベントを追加します。
グループ変化イベントは、オブジェクトが持っているパラメーターの時間による変化を設定します。
frameで指定したフレーム数が経過した時に(x1,y1,z1)の値になります。
swの補間オプションは、以下の値を指定することができます。
^p
	sw = 0 : リニア補間(絶対値)
	sw = 1 : スプライン補間(絶対値)
	sw = 2 : リニア補間(相対値)
	sw = 3 : スプライン補間(相対値)
^p
swを省略した場合には、絶対値リニアが設定されます。

%href
event_pos
event_ang
event_angr
event_scale
event_dir
event_work
newevent
setevent


%index
event_work
workグループ変化イベントを追加
%group
拡張画面制御命令
%prm
id,frame,x1,y1,z1,sw
id         : イベントID
frame      : 変化までのフレーム数
(x1,y1,z1) : 設定される値
sw(0)      : 補間オプション
%inst
idで指定しているイベントIDに、グループ変化イベントを追加します。
グループ変化イベントは、オブジェクトが持っているパラメーターの時間による変化を設定します。
frameで指定したフレーム数が経過した時に(x1,y1,z1)の値になります。
swの補間オプションは、以下の値を指定することができます。
^p
	sw = 0 : リニア補間(絶対値)
	sw = 1 : スプライン補間(絶対値)
	sw = 2 : リニア補間(相対値)
	sw = 3 : スプライン補間(相対値)
^p
swを省略した場合には、絶対値リニアが設定されます。

%href
event_pos
event_ang
event_angr
event_scale
event_dir
event_efx
newevent
setevent


%index
event_addpos
posグループ加算イベントを追加
%group
拡張画面制御命令
%prm
id,x,y,z
id      : イベントID
(x,y,z) : 加算される値
%inst
idで指定しているイベントIDに、グループ加算イベントを追加します。
グループ加算イベントは、オブジェクトが持っているパラメーターに(x,y,z)の値を加算します。

%href
event_addang
event_addangr
event_addscale
event_adddir
event_addefx
event_addwork
newevent
setevent


%index
event_addang
angグループ加算イベントを追加
%group
拡張画面制御命令
%prm
id,x,y,z
id      : イベントID
(x,y,z) : 加算される値
%inst
idで指定しているイベントIDに、グループ加算イベントを追加します。
グループ加算イベントは、オブジェクトが持っているパラメーターに(x,y,z)の値を加算します。

%href
event_addpos
event_addangr
event_addscale
event_adddir
event_addefx
event_addwork
newevent
setevent


%index
event_addangr
angグループ加算イベントを追加
%group
拡張画面制御命令
%prm
id,x,y,z
id      : イベントID
(x,y,z) : 加算される値
%inst
idで指定しているイベントIDに、グループ加算イベントを追加します。
グループ加算イベントは、オブジェクトが持っているパラメーターに(x,y,z)の値を加算します。

%href
event_addpos
event_addang
event_addscale
event_adddir
event_addefx
event_addwork
newevent
setevent


%index
event_addscale
scaleグループ加算イベントを追加
%group
拡張画面制御命令
%prm
id,x,y,z
id      : イベントID
(x,y,z) : 加算される値
%inst
idで指定しているイベントIDに、グループ加算イベントを追加します。
グループ加算イベントは、オブジェクトが持っているパラメーターに(x,y,z)の値を加算します。

%href
event_addpos
event_addang
event_addangr
event_adddir
event_addefx
event_addwork
newevent
setevent


%index
event_adddir
dirグループ加算イベントを追加
%group
拡張画面制御命令
%prm
id,x,y,z
id      : イベントID
(x,y,z) : 加算される値
%inst
idで指定しているイベントIDに、グループ加算イベントを追加します。
グループ加算イベントは、オブジェクトが持っているパラメーターに(x,y,z)の値を加算します。

%href
event_addpos
event_addang
event_addangr
event_addscale
event_addefx
event_addwork
newevent
setevent


%index
event_addefx
efxグループ加算イベントを追加
%group
拡張画面制御命令
%prm
id,x,y,z
id      : イベントID
(x,y,z) : 加算される値
%inst
idで指定しているイベントIDに、グループ加算イベントを追加します。
グループ加算イベントは、オブジェクトが持っているパラメーターに(x,y,z)の値を加算します。

%href
event_addpos
event_addang
event_addangr
event_addscale
event_adddir
event_addwork
newevent
setevent


%index
event_addwork
workグループ加算イベントを追加
%group
拡張画面制御命令
%prm
id,x,y,z
id      : イベントID
(x,y,z) : 加算される値
%inst
idで指定しているイベントIDに、グループ加算イベントを追加します。
グループ加算イベントは、オブジェクトが持っているパラメーターに(x,y,z)の値を加算します。

%href
event_addpos
event_addang
event_addangr
event_addscale
event_adddir
event_addefx
newevent
setevent


%index
setevent
オブジェクトにイベントを設定
%group
拡張画面制御命令
%prm
p1,p2,p3
p1(0)  : オブジェクトID
p2(0)  : イベントID
p3(-1) : イベントスロットID
%inst
p1で指定したオブジェクトにp2のイベントを適用します。
あらかじめ、決まった流れの処理(イベント)を登録したイベントリストを用意しておく必要があります。
^
setevent命令によって設定されるイベントは、オブジェクト１つあたり４つまで同時に適用することが可能です。
p3にイベントを設定するためのイベントスロットID(0から3まで)を指定することができます。
p3を省略するか、-1を指定した場合には0から順番に空いているイベントスロットIDが使用されます。
オブジェクトに設定されたイベントを削除する場合には、p3にイベントスロットIDを指定して、p2をマイナス値にしてください。
^
イベントの設定に成功した場合には、システム変数statに設定されたイベントスロットIDが代入されます。
イベントの設定に失敗すると、システム変数statに-1が代入されます。


%href
newevent


%index
delevent
イベントリストを削除
%group
拡張画面制御命令
%prm
p1
p1 : イベントID
%inst
p1で指定したイベントリストを削除します。

%href
newevent


%index
newevent
イベントリストを作成
%group
拡張画面制御命令
%prm
p1
p1 : イベントIDが代入される変数名
%inst
新しいイベントIDを取得し、p1で指定した変数に代入します。
^
新しくイベントを作成する場合には、必ずnewevent命令でイベントIDを取得しておく必要があります。
次に、「event_」で始まるイベントリスト追加命令によって多彩な動作を登録しておくことができます。
一度取得されたイベントIDは、シーンのリセット(hgreset命令)が行なわれるか、
またはdelevent命令によってイベントリストが削除されるまでは保持されます。
^
こうしてできたイベントは、setevent命令によっていつでもオブジェクトに対して適用することができます。


%href
delevent
setevent


%index
cammode
カメラモードの設定
%group
拡張画面制御命令
%prm
mode
mode : モード値
%inst
カメラの向きについてのモードを設定します。
指定できるモードは、
CAM_MODE_NORMAL		(カメラの位置、角度に従う)
と、
CAM_MODE_LOOKAT		(カメラは注視点を常に向いている)
です。
%href
selcam
selcpos
selcint


%index
settoonedge
トゥーンシェーディング時の設定
%group
拡張画面制御命令
%prm
p1,p2,p3,p4
p1    : モデルID
p2(0) : エッジの色(RGBコード)
p3(0) : Zオフセット
p4(0) : Xオフセット(0.0〜1.0)
%inst
p1で指定したモデルのトゥーンシェーディング時の設定を行ないます。
p2で、エッジの色コード(RRGGBB)を指定します。
p3,p4は実数値によりエッジのオフセットを指定することができます。
Zオフセット(p3)を変更することにより、輪郭線の太さを調整することができます。
Zオフセットのデフォルト値は、0.005が設定されています。
Xオフセット(p4)は、トゥーンシェーディングで使用するテクスチャのX方向原点を指定します。
Xオフセットのデフォルト値は、0.5が設定されており、この場合はX方向の半分にあたる位置を中心にして、テクスチャのU値が計算されることになります。

%href
loadtoon
maketoon


%index
event_uv
UV設定イベントを追加
%group
拡張画面制御命令
%prm
p1,p2,p3
p1    : イベントID
p2    : Xオフセット
p3    : Yオフセット
%inst
idで指定しているイベントIDに、UV設定イベントを追加します。
UV設定イベントは、2D及び3D(x形式モデルを除く)の単一ポリゴンが持つUVを動的に変更します。
これは、テクスチャーアニメーションを実現させるためのもので、イベントが実行されると、テクスチャのUV座標が(p2,p3)で指定された値(ドット数)に再設定されます。

%href
newevent
setevent


%index
setobjmodel
オブジェクトのモデル設定
%group
拡張画面制御命令
%prm
ObjID,modelID
ObjID    : オブジェクトID
modelID  : モデルID
%inst
指定されたオブジェクトが表示するモデルを変更します。
%href
regobj
setobjmode


%index
event_wpos
posグループ変化イベントを追加
%group
拡張画面制御命令
%prm
id,frame,sw
id         : イベントID
frame      : 変化までのフレーム数
sw(1)      : 補間オプション
%inst
idで指定しているイベントIDに、グループ変化イベントを追加します。
グループ変化イベントは、オブジェクトが持っているパラメーターの時間による変化を設定します。
frameで指定したフレーム数が経過した時にworkグループ(ワーク値)の値になります。
swの補間オプションは、以下の値を指定することができます。
^p
	sw = 0 : リニア補間(絶対値)
	sw = 1 : スプライン補間(絶対値)
	sw = 2 : リニア補間(相対値)
	sw = 3 : スプライン補間(相対値)
^p
swを省略した場合には、絶対値スプラインが設定されます。

%href
event_wang
event_wangr
event_wscale
event_wdir
event_wefx
newevent
setevent


%index
event_wang
angグループ変化イベントを追加
%group
拡張画面制御命令
%prm
id,frame,sw
id         : イベントID
frame      : 変化までのフレーム数
sw(0)      : 補間オプション
%inst
idで指定しているイベントIDに、グループ変化イベントを追加します。
グループ変化イベントは、オブジェクトが持っているパラメーターの時間による変化を設定します。
frameで指定したフレーム数が経過した時にworkグループ(ワーク値)の値になります。
swの補間オプションは、以下の値を指定することができます。
^p
	sw = 0 : リニア補間(絶対値)
	sw = 1 : スプライン補間(絶対値)
	sw = 2 : リニア補間(相対値)
	sw = 3 : スプライン補間(相対値)
^p
swを省略した場合には、絶対値リニアが設定されます。

%href
event_wpos
event_wangr
event_wscale
event_wdir
event_wefx
newevent
setevent


%index
event_wscale
scaleグループ変化イベントを追加
%group
拡張画面制御命令
%prm
id,frame,sw
id         : イベントID
frame      : 変化までのフレーム数
sw(0)      : 補間オプション
%inst
idで指定しているイベントIDに、グループ変化イベントを追加します。
グループ変化イベントは、オブジェクトが持っているパラメーターの時間による変化を設定します。
frameで指定したフレーム数が経過した時にworkグループ(ワーク値)の値になります。
swの補間オプションは、以下の値を指定することができます。
^p
	sw = 0 : リニア補間(絶対値)
	sw = 1 : スプライン補間(絶対値)
	sw = 2 : リニア補間(相対値)
	sw = 3 : スプライン補間(相対値)
^p
swを省略した場合には、絶対値リニアが設定されます。

%href
event_wpos
event_wang
event_wangr
event_wdir
event_wefx
newevent
setevent


%index
event_wdir
dirグループ変化イベントを追加
%group
拡張画面制御命令
%prm
id,frame,sw
id         : イベントID
frame      : 変化までのフレーム数
sw(0)      : 補間オプション
%inst
idで指定しているイベントIDに、グループ変化イベントを追加します。
グループ変化イベントは、オブジェクトが持っているパラメーターの時間による変化を設定します。
frameで指定したフレーム数が経過した時にworkグループ(ワーク値)の値になります。
swの補間オプションは、以下の値を指定することができます。
^p
	sw = 0 : リニア補間(絶対値)
	sw = 1 : スプライン補間(絶対値)
	sw = 2 : リニア補間(相対値)
	sw = 3 : スプライン補間(相対値)
^p
swを省略した場合には、絶対値リニアが設定されます。

%href
event_wpos
event_wang
event_wangr
event_wscale
event_wefx
newevent
setevent


%index
event_wefx
efxグループ変化イベントを追加
%group
拡張画面制御命令
%prm
id,frame,sw
id         : イベントID
frame      : 変化までのフレーム数
sw(0)      : 補間オプション
%inst
idで指定しているイベントIDに、グループ変化イベントを追加します。
グループ変化イベントは、オブジェクトが持っているパラメーターの時間による変化を設定します。
frameで指定したフレーム数が経過した時にworkグループ(ワーク値)の値になります。
swの補間オプションは、以下の値を指定することができます。
^p
	sw = 0 : リニア補間(絶対値)
	sw = 1 : スプライン補間(絶対値)
	sw = 2 : リニア補間(相対値)
	sw = 3 : スプライン補間(相対値)
^p
swを省略した場合には、絶対値リニアが設定されます。

%href
event_wpos
event_wang
event_wangr
event_wscale
event_wdir
newevent
setevent


%index
event_delobj
オブジェクト削除イベントを追加
%group
拡張画面制御命令
%prm
id
id         : イベントID
%inst
idで指定しているイベントIDに、オブジェクト削除イベントを追加します。
オブジェクト削除イベントは、現在イベントを実行しているオブジェクトそのものを削除する命令です。
%href
event_regobj
newevent
setevent


%index
event_regobj
オブジェクト生成イベントを追加
%group
拡張画面制御命令
%prm
id,model,event
id         : イベントID
model      : モデルID
event      : イベントID
%inst
idで指定しているイベントIDに、オブジェクト生成イベントを追加します。
オブジェクト生成イベントは、regobj命令と同様にイベント実行時に新しいオブジェクトを生成します。
パラメーターとして、modelにモデルIDを、eventにイベントIDを指定することができます。また、生成されるオブジェクトの座標は、イベントを実行しているオブジェクトの座標が継承されます。

%href
event_delobj
newevent
setevent


%index
event_eprim
エフェクト設定イベントを追加
%group
拡張画面制御命令
%prm
id,model,param,val1,val2
id     : イベントID
model  : モデルID
param  : パラメーターID
val1   : 設定値1
val2   : 設定値2
%inst
idで指定しているイベントIDに、エフェクト設定イベントを追加します。
seteprim命令と同様の操作をイベント実行時に行なうことができます。
modelに、addeprim命令によって作成されたモデルIDを指定します。
モデルIDがマイナス値の場合は、イベントが設定されているオブジェクトに割り当てられているモデルが対象になります。
paramにseteprim命令と同様のパラメーターIDを指定します。
ただし、色指定(パラメーターID16以降)については、A→R→G→Bの順番で別なパラメーターIDが割り当てられています。
(seteprim命令では、16にあたるIDが16(A),17(R),18(G),19(B)に分割されています。)
val2を省略した場合には、val1の値が設定されます。
val2が指定された場合は、実際に設定される値がval1〜val2になります。
val1〜val2は、イベント実行時に乱数で生成されます。

%href
addeprim
seteprim
newevent
setevent


%index
selcam
カメラをMOC情報に設定
%group
拡張画面制御命令
%prm
mocofs
mocofs : MOCのグループ指定
%inst
MOC設定命令の対象となるMOCグループをカメラに設定します。
通常は、selcpos,selcang,selcint命令をお使いください。
%href
selmoc
selpos
selang
selscale
seldir
selcpos
selcang
selcint


%index
sellight
光源をMOC情報に設定
%group
拡張画面制御命令
%prm
id,ofs
id     : 光源のID(0〜3)
mocofs : MOCのグループ指定
%inst
MOC設定命令の対象となるMOCグループをライト(光源)に設定します。
通常は、sellpos,sellang,sellcolor命令をお使いください。
%href
sellpos
sellang
sellcolor
selmoc
selcam


%index
selcpos
カメラ座標をMOC情報に設定
%group
拡張画面制御命令
%prm
%inst
MOC設定命令の対象となるMOCグループをカメラのA(移動座標)に設定します
%href
selmoc
selpos
selang
selscale
seldir
selcam
selcang
selcint


%index
selcang
カメラ角度をMOC情報に設定
%group
拡張画面制御命令
%prm
%inst
MOC設定命令の対象となるMOCグループをカメラのB(回転角度)に設定します
cammodeの設定が注視点を見るモードになっている場合は、カメラ回転角は自動的に設定されます。
%href
cammode
selmoc
selpos
selang
selscale
seldir
selcam
selcpos
selcint


%index
selcint
カメラ注視点をMOC情報に設定
%group
拡張画面制御命令
%prm
%inst
MOC設定命令の対象となるMOCグループをC(注視点座標)に設定します
注視点は、cammodeの設定によって有効になります。
%href
cammode
selmoc
selpos
selang
selscale
seldir
selcam
selcpos
selcang


%index
sellpos
光源座標をMOC情報に設定
%group
拡張画面制御命令
%prm
id
id     : 光源のID(0〜3)
%inst
MOC設定命令の対象となるMOCグループをライト(光源)の座標に設定します。
%href
sellight
sellang
sellcolor
sellambient


%index
sellang
光源角度をMOC情報に設定
%group
拡張画面制御命令
%prm
id
id     : 光源のID(0〜3)
%inst
MOC設定命令の対象となるMOCグループをライト(光源)の角度に設定します。
%href
sellight
sellpos
sellcolor
sellambient


%index
sellcolor
光源色をMOC情報に設定
%group
拡張画面制御命令
%prm
id
id     : 光源のID(0〜3)
%inst
MOC設定命令の対象となるMOCグループをライト(光源)の色(R,G,B,Ambient)に設定します。
%href
sellight
sellpos
sellang
sellambient


%index
sellambient
環境光色をMOC情報に設定
%group
拡張画面制御命令
%prm
id
id     : 光源のID(0〜3)
%inst
MOC設定命令の対象となるMOCグループをアンビエント(環境光)の色(R,G,B)に設定します。
%href
sellight
sellpos
sellang
sellcolor


%index
objgetv
MOC情報を整数値で取得
%group
拡張画面制御命令
%prm
v
v    : 値が代入される変数名
%inst
MOCに設定されている値を変数vに整数で代入します。
v.0〜v.3にそれぞれのオフセット番号が持つ値が代入されます。
%href
objsetv
objsetfv
objgetfv
fvset
fvadd
fvsub
fvmul
fvdiv


%index
objsetv
MOC情報を整数値で設定
%group
拡張画面制御命令
%prm
v
v    : 値が代入されている変数名
%inst
変数vに整数で代入されている値をMOCに設定します。
v.0〜v.3がそれぞれのオフセット番号に設定されます。
%href
objgetv
objsetfv
objgetfv
fvset
fvadd
fvsub
fvmul
fvdiv


%index
objgetstr
MOC情報を文字列で取得
%group
拡張画面制御命令
%prm
sval
sval : 結果が代入される文字列型変数名
%inst
MOCに設定されている値を変数svalに文字列で書き出します。
%href
fv2str
f2str


%index
objact
指定アニメーションを開始
%group
拡張画面制御命令
%prm
ObjID,Anim,mode
ObjID   : オブジェクトID
Anim    : アニメーションインデックス値
mode(0) : 設定モード(0=即時/1=終了時)
%inst
指定されたオブジェクトのアニメーションを開始します。
Animで複数あるアニメーションを示すインデックス値を指定します。
インデックス値は、複数アニメーション読み込みを行なった順番に、0,1,2,3…という数字で表わされます。
Animにマイナスの値(-1)を指定することで、アニメーションを解除(停止)させることができます。
また、modeの値により設定のモードを指定することができます。
設定モードを0または省略した場合は、即時にアニメーションを切り替えます。
設定モードを1に指定した場合は、現在再生されているアニメーションが終わるのを待ってから、切り替えを行ないます。
設定モード1で、Animにマイナス値を指定した場合は、現在のアニメーション再生終了後にアニメーションを停止します。
設定モード0でアニメーションを再生した場合は、再生スピードはデフォルト値に戻され、リピート再生が行なわれる設定になります。
アニメーションの開始時は、モデルが持つ標準的なアニメーション速度(modelspeed命令にて変更可能)が設定されます。
アニメーションの再生中は、objspeed命令により速度を変更することが可能です。

%href
addxfile
addxanim
objspeed
modelspeed
getanim


%index
getanim
アニメーション情報を取得
%group
拡張画面制御命令
%prm
p1,p2,p3
p1    : 情報が代入される変数
p2(0) : オブジェクトID
p3(0) : 情報ID
%inst
p2で指定されたオブジェクトのアニメーションに関する情報を取得して、p1で指定された変数に代入します。
取得される情報の種類は、p3の情報IDで指定することができます。
情報IDは、以下の値が用意されています。
^p
情報ID   内容
-----------------------------------------------
  0      アニメーション開始からの時間(ms)
  1      アニメーションループフラグ
^p
結果が代入される変数は、自動的に整数型に設定されます。
アニメーションループフラグは、アニメーション開始時は0ですが、最後まで再生された場合には1が返されます。

%href
objact


%index
addxfile
Xファイルモデルを作成
%group
拡張画面制御命令
%prm
var,"filename"
var        : 作成されたモデルIDが代入される変数名
"filename" : 読み込みを行なうファイル名
%inst
x形式のモデルファイルを読み込みます。
正常にモデルが読みこまれると、varで指定した変数にモデルIDが代入されます。
読み込みに失敗した場合には、varで指定した変数に-1が代入されます。

HGIMG3が読み込むx形式は、DirectX8以降のスキンメッシュに対応しています。
ワンスキンモデル及びアニメーションを出力可能な各種ツールで、x形式の出力に対応したものを別途ご用意下さい。

setreq命令によって法線再計算スイッチ(SYSREQ_CALCNORMAL)が0以外に設定
されている場合には、モデル法線の再計算を行ないます。
通常は、x形式ファイルに記録された法線をそのまま使用します。

%href
addxanim



%index
addxanim
Xファイルアニメーションの追加
%group
拡張画面制御命令
%prm
ModelID,"filename"
ModelID    : モデルID
"filename" : 読み込みを行なうファイル名
%inst
Xファイルモデルに、アニメーションデータを追加します。
ModelIDでは、すでにaddxfile命令によって読み込まれているモデルIDを指定する必要があります。
アニメーションの追加に成功すると、システム変数statにモデルIDが代入されます。
アニメーションの追加に失敗した場合は、システム変数statに-1が代入されます。
^
addxanim命令は、１つのモデルで複数のアニメーションを切り替えて使用する場合に利用します。
最初にモデル全体と最初のアニメーションを含んだデータをaddxfile命令により読み込み、さらに追加でアニメーションだけを登録する場合にaddxanim命令を使用します。
必ず、モデルやボーンの構成は同一のものから出力されている必要があります。

複数のアニメーションは、objact命令によって切り替えることが可能です。
切り替えるためのアニメーションインデックス値は、最初にaddxfile命令で読み込まれたアニメーションを0として、以降addxanim命令で追加するたびに1,2,3…と増加していきます。
^
追加できるアニメーションの最大数は、標準で16(もとのxファイル含む)です。
最大数を拡張する場合は、setreq命令によってアニメーション最大数(SYSREQ_MDLANIM)を適切に設定してください。
^
複数のアニメーション情報を持つXファイルからは、最初の１つしか読み込むことができません。複数読み込む際には、複数のXファイルを用意する必要があります。

%href
addxfile
objact


%index
objspeed
アニメーション再生速度の設定
%group
拡張画面制御命令
%prm
p1,p2
p1(0) : オブジェクトID
p2(0) : １フレームあたりの再生速度(実数値)
%inst
p1で指定されたオブジェクトIDのアニメーション再生速度を設定します。
p2で１フレームあたりの再生速度(ms)を実数値で指定することができます。
この命令は、objact命令により再生されているアニメーションの再生速度を変更するためのものです。
objact命令を実行した場合は、modelspeed命令によって設定されている標準のアニメーション再生速度が適用されます。
アニメーションの再生前に速度を設定する場合は、modelspeed命令を使用してください。

%href
modelspeed
objact




%index
modelspeed
アニメーション再生速度の設定(2)
%group
拡張画面制御命令
%prm
p1,p2
p1(0) : モデルID
p2(0) : １フレームあたりの再生速度(実数値)
%inst
p1で指定されたモデルIDが持つ標準のアニメーション再生速度を設定します。
p2で１フレームあたりの再生速度(ms)を実数値で指定することができます。
以降は、指定されたモデルを使用するオブジェクトが、アニメーションを再生する場合の標準速度として設定されます。
modelspeed命令によって標準のアニメーション再生速度が設定されなかった場合は、15(ms)がオブジェクト初期化時に設定されます。
objact命令により、すでに再生されているアニメーションの再生速度は、objspeed命令によって変更してください。


%href
objspeed
objact


%index
hgcapture
DirectX画面データの取得
%group
拡張画面制御命令
%prm

%inst
HGIMGが描画しているフレームバッファの情報をHSPの画面にコピーします。
gsel命令などで現在アクティブな画面がコピー先になります。
必ず、HGIMGを初期化したサイズと同じ大きさのバッファをコピー先にしてください。
hgcapture命令は、HSPの仮想画面バッファにコピーを行ないますが、画面の更新は行なわれません。
取得された画面をウィンドウ上で反映させるためには、redraw命令により再描画を行なう必要があります。
HGIMGを初期化したウィンドウIDと同じバッファをコピー先にした場合は、HGIMGの画面がオーバーレイされているため、内容が表示されないので注意してください。
また、hgcapture命令は、ビデオカードのフレームバッファから大量のデータをメインメモリにコピーするため、ビデオカードやCPU負荷が発生します。

%href
redraw



%index
objlight
参照されるライトの設定
%group
拡張画面制御命令
%prm
p1,p2,p3
p1(0)  : ライトが設定されるオブジェクトID
p2(0)  : ライトとして参照されるオブジェクトID
p3(-1) : 設定モード
%inst
p1で指定されたオブジェクトIDが影響を受けるライトを設定します。
p2で参照されるオブジェクトIDを指定します。
通常、すべてのオブジェクトは基本ライト(HGOBJ_LIGHT)の影響を受けます。
objlight命令により、基本ライト以外のオブジェクトから影響を受ける状態に変更します。
参照される追加のライトオブジェクトは、reglight命令で作成することができます。

p3でモードを指定することができます。p3の値は、以下の意味を持ちます。
^p
モード   内容
---------------------------------------
   1     ライトの方向
   2     ライトの色
   4     ライトのアンビエント色
^p
複数の項目を設定する場合は、それぞれのモード値を加算してください。
p3を省略すると、方向・色・アンビエント色のすべてが設定されます。

%href
reglight



%index
reglight
ライトオブジェクトの登録
%group
拡張画面制御命令
%prm
p1
p1 : 作成されたオブジェクトIDが代入される変数名
%inst
追加のライトオブジェクトを登録して、p1に指定された変数にIDを代入します。
ライトオブジェクトは、objlight命令によって新しく参照されるためのライトとして扱うことができます。
取得されたオブジェクトIDは、他のオブジェクトと同じ方法でパラメーターを設定、取得することが可能です。
設定できるパラメーターは、基本ライト(HGOBJ_LIGHT)と同様のものになります。

%href
objlight



%index
getxinfo
Xモデル表示情報の取得
%group
拡張画面制御命令
%prm
p1,p2,p3
p1 : 作成されたモデル情報文字列が代入される変数名
p2 : オブジェクトID
p3 : ノードID
p4 : 設定モード

%inst
p2で指定されたオブジェクトIDが示すモデルの情報を文字列として
p1で指定した変数に代入します。
必ずX形式のモデル持つオブジェクトIDを指定する必要があります。
p1の変数には、文字列の形で情報が代入されます。
p2でノードIDを指定することができます。
ノードIDは0から始まる数値で、ボーンやメッシュごとに複数のIDが割り振られています。
ノードIDに-1などマイナス値を指定した場合は、改行コードで区切られた1行1要素の形ですべてのノード情報が返されます。
これによりノードIDの最大数を調べることができます。
(ノードIDの最大数は、同じモデルであればモードが違っていても共通です。)
p4で取得する情報の種類を指定することができます。
p4で指定できる値は、以下の通りです。
^p
モード        内容
---------------------------------------------
 0            ノード名
 1            ノードのワールド座標
 2            ノードのX,Y,Z回転角度
 16           ノードが持つマテリアル数
 $1000+MatID  マテリアルの色( Diffuse Color )
 $2000+MatID  テクスチャのCOMポインタ(テクスチャ未使用時は0)
 $10000+Index  頂点座標
 $20000+Index  UV座標
^p
$1000と$2000は、マテリアルIDと組み合わせて使用します。
マテリアルIDは、ノードごとに割り振られた0から始まる値で、有効なマテリアルIDの数は、モード16で調べることができます。
^
$10000と$20000は、内部バッファのIndexと組み合わせて使用します。
指定したインデックスが持つ頂点座標またはUV座標の値を取得することができます。
^
getxinfo命令は、ボーンを含むモデルが持つ個別の位置情報などを得るために使用することができます。
多数のノードを持つモデルを処理する場合には、処理がかかることもあるのでリアルタイムな動作を行なうアプリケーションでは注意して下さい。
現在は、モード1及び2の値は最後に描画されたモデルに関する情報を取得します。
複数のオブジェクトでモデルを共有している場合は、正しく取得されませんのでご注意ください。

%href
setxinfo



%index
setxinfo
Xモデル情報の詳細設定
%group
拡張画面制御命令
%prm
p1,p2,p3,x,y,z
p1 : オブジェクトID
p2 : ノードID
p3 : 設定モード
(x,y,z) : 設定されるベクトル情報(実数値)

%inst
p1で指定されたオブジェクトIDが示すモデルの詳細情報を設定します。
必ずX形式のモデル持つオブジェクトIDを指定する必要があります。
モデルが複数のノード(ボーンやメッシュなど)を持っている場合には、p2でノードIDを指定することができます。
ノードIDは0から始まる数値で、ボーンやメッシュごとに複数のIDが割り振られています。
(指定可能なノードIDの範囲は、getxinfo命令によってノード一覧を取得して調べることができます。)
p3で取得する情報の種類を指定することができます。
p3で指定できる値は、以下の通りです。
^p
モード        内容
---------------------------------------------
 1            ノードのローカル座標(x,y,z)
 2            ノードの回転角度(x,y,z)
 $1000+MatID  マテリアルの色( xyz=RGB )
 $2000+MatID  参照されるテクスチャ( x=テクスチャID )
 $10000+Index  頂点座標( x,y,z )
 $20000+Index  UV座標( x,y )
^p
$1000と$2000は、マテリアルIDと組み合わせて使用します。
マテリアルIDは、ノードごとに割り振られた0から始まる値で、有効なマテリアルIDの数は、getxinfo命令のモード16で調べることができます。
^
$10000と$20000は、内部バッファのIndexと組み合わせて使用します。
指定したインデックスが持つ頂点座標またはUV座標を直接編集することができます。
^
setxinfo命令は、ボーンを含むモデルが持つ個別の状態を変更するために使用することができます。
アニメーションが設定されているノードは設定することができません。
また、正しいノードIDを指定しないと表示が崩れることもあるので、十分に注意して使用してください。

%href
getxinfo



%index
getobjmodel
オブジェクトのモデルID取得
%group
拡張画面制御命令
%prm
var,id
var      : 結果が代入される変数名
id       : オブジェクトID
%inst
指定したオブジェクトに割り当てられているモデルIDを取得し、varで指定された変数に代入します。

%href
setobjmodel



%index
modelcols
コリジョンパラメーターの設定(2)
%group
拡張画面制御命令
%prm
p1,p2
p1(0) : モデルID
p2(1.0) : コリジョンパラメーターX(実数値)
p3(1.0) : コリジョンパラメーターY(実数値)
p4(1.0) : コリジョンパラメーターZ(実数値)
p5(0) : 設定タイプ
%inst
p1で指定されたモデルIDが持つ標準のコリジョンパラメーターを設定します。
以降は、指定されたモデルを使用するオブジェクト初期化時のコリジョンパラメーターとして設定されます。
p5の設定タイプが0の場合は、getcoli命令のコリジョン判定時に、
指定された値に対するスケール(倍率)を実数でX,Y,Z軸ごとに指定することができます。
(標準のコリジョンスケールが設定されなかった場合は、1.0(等倍)が設定されます。)
p5の設定タイプが1の場合は、拡張コリジョンパラメーターの設定となります。
p2をオブジェクトの重さ(反発力)、p3を高さ、p4を半径とした円柱をコリジョン判定用に設定します。
(これは、objwalk命令による地形判定用で使用するための設定です)

オブジェクトごとのコリジョンパラメーターは、setcolscale命令によって設定することも可能です。

%href
setcolscale




%index
setcolscale
コリジョンパラメーターの設定
%group
拡張画面制御命令
%prm
p1,p2
p1(0) : オブジェクトID
p2(1.0) : コリジョンパラメーターX(実数値)
p3(1.0) : コリジョンパラメーターY(実数値)
p4(1.0) : コリジョンパラメーターZ(実数値)
p5(0) : 設定タイプ
%inst
p1で指定されたオブジェクトIDが持つコリジョンパラメーターを設定します。
p5の設定タイプが0の場合は、getcoli命令のコリジョン判定時に、指定された値に対するスケール(倍率)を実数でX,Y,Z軸ごとに指定することができます。
(標準のコリジョンスケールが設定されなかった場合は、1.0(等倍)が設定されます。)
p5の設定タイプが1の場合は、拡張コリジョンパラメーターの設定となります。
p2をオブジェクトの重さ(反発力)、p3を高さ、p4を半径とした円柱をコリジョン判定用に設定します。
(これは、objwalk命令による地形判定用で使用するための設定です)

setcolscale命令によってコリジョンパラメーターが設定されなかった場合は、モデルごとの標準コリジョンパラメーターが使用されます。

%href
modelcols



%index
modelshade
シェーディングモードの設定
%group
拡張画面制御命令
%prm
p1,p2
p1(0) : モデルID
p2(0) : シェーディングモード
%inst
p1で指定されたモデルIDが持つシェーディングモードを設定します。
シェーディングモードは、モデルを表示した時に行なう光源計算の方法を示します。
モード値は、以下のいずれかになります。
^p
	モード値      内容
	-----------------------------------------------------
	   0          コンスタント(光源計算なし)
	   1          グーロー(DirectX標準の光源計算)
	   2          半球ライティング
^p
モード値が0の場合は、光源計算を行なわず常にマテリアル色をそのまま反映します。(コンスタントシェーディング)
モード値が0の場合は、ライト色、アンビエント色を考慮した光源計算を行ないます。この場合の表示色は、
^p
	輝度 = 光の強さ * ライト色 * マテリアル色 + アンビエント色
^p
という式が使用されます。(輝度が255を越えた場合は、255になります。)
モード値が2の場合は、ライト色を天球色、アンビエント色を地上色として半球ライティングの計算が行なわれます。
半球ライティングは、DirectX標準の光源計算と比べてやわらかできめの細かい陰影を表現することができます。
現在のバージョンでは、シェーディングモードの設定は、x形式のモデルにのみ適用することができます。
また、シェーディングモード１ではライトのscale(カラー)パラメータは現在反映されない仕様となっていますのでご了承ください。

%href


%index
modelorder
回転順序モードの設定
%group
拡張画面制御命令
%prm
p1,p2
p1(0) : モデルID
p2(0) : 回転順序モード
%inst
p1で指定されたモデルIDが持つ回転順序のモードを設定します。
回転順序モードにより、3Dオブジェクト表示時の回転順序を設定することができます。
モードの値は、以下のいずれかになります。
^p
	モード値  マクロ                 回転順序
	------------------------------------------------------
	    0     HGMODEL_ROTORDER_ZYX   Z→Y→X
	    1     HGMODEL_ROTORDER_XYZ   X→Y→Z
	    2     HGMODEL_ROTORDER_YXZ   Y→X→Z
^p
デフォルトのモデルでは、モード値0が設定されています。
(ただし、x形式のモデルのみモード値1がデフォルトとなります。)

modelorder命令は、どうしても解決できないオブジェクトの向きを設定する場合や、既存のデータとの互換を取るために用意されています。
通常、使用する範囲で必須となる命令ではありません。

%href


%index
objchild
オブジェクトの親子関係設定
%group
拡張画面制御命令
%prm
ObjID,ChildObjID
ObjID      : オブジェクトID
ChildObjID : 子供のオブジェクトID
%inst
指定されたオブジェクトの子供となるオブジェクトを設定します。
ChildObjIDで指定されたオブジェクトは、ObjIDで指定されたオブジェクトと親子関係を持つことになります。
以降は親となる、ObjIDで指定されたオブジェクトの座標、回転角度、スケールを継承した形で表示が行なわれます。
また、親のオブジェクトが削除された場合には、その子供すべても同時に削除されます。
子供を持っている親オブジェクトを、ChildObjIDに指定することで多階層の構造を設定することが可能です。
すでに何らかの親を持っているオブジェクトを子供に指定することはできません。
ChildObjIDにマイナス値を指定すると、ObjIDで指定したオブジェクトが持つ親子設定をすべて破棄します。
^
現在のバージョンでは、x形式のモデルを持つオブジェクトでの親子関係設定は反映されません。x形式以外の3D,2Dモデルでのみご使用ください。
%href
regobj
delobj


%index
objproj
オブジェクトのプロジェクション変更
%group
拡張画面制御命令
%prm
ObjID,Mode
ObjID      : オブジェクトID
Mode       : プロジェクションモード
%inst
ObjIDで指定されたオブジェクトのプロジェクションモードを変更します。
プロジェクションモードが0の場合は、通常のオブジェクトとしてカメラのNearZ値、FarZ値に従ってZ方向のクリッピングが行なわれます。
プロジェクションモードが1の場合は、特殊なオブジェクトとしてカメラのFarZ値は無視されます。
背景など、一部のオブジェクトだけを遠くに描画するような場合に、プロジェクションモードを変更することができます。
%href
regobj


%index
dmmini
サウンド機能初期化
%group
拡張マルチメディア制御命令
%prm
%inst
サウンド機能の初期化を行ないます。
プログラム実行時の最初に１回だけ初期化を行なう必要があります。
これ以降は、dmm〜で始まる命令を使用して、DirectSoundによるサウンド再生機能を使用することが可能になります。
dmmini命令により、初期化が行なわれた時にのみサウンド機能が使用されます。
HGIMG3使用時でも、サウンド関連機能の初期化が行なわれない場合は、DirectSoundなどサウンドに関するAPIが使用されることはありません。
HGIMG3と他のサウンド関連プラグインを使用する場合は、通常通りお使い頂けます。

%href
dmmreset
dmmbye


%index
dmmreset
サウンド機能の設定をリセット
%group
拡張マルチメディア制御命令
%prm
%inst
サウンド機能の設定をリセットします。
読み込まれていた、すべてのサウンドデータは破棄されます。

%href
dmmini
dmmbye


%index
dmmbye
サウンド機能終了処理
%group
拡張マルチメディア制御命令
%prm
%inst
サウンド機能の終了処理を行ないます。
通常は、終了処理が自動的に行なわれるので、この命令を入れる必要はありません。

%href
dmmreset
dmmini


%index
dmmdel
サウンドデータを削除
%group
拡張マルチメディア制御命令
%prm
p1
p1(0) : サウンドID
%inst
p1で指定したサウンドIDの情報を破棄します。
%href
dmmload


%index
dmmvol
サウンドデータの音量設定
%group
拡張マルチメディア制御命令
%prm
p1,p2
p1(0) : サウンドID
p2(0) : 音量(-10000〜0)
%inst
p1で指定したサウンドIDの音量(ボリューム)を設定します。
p2で設定する音量を指定します。0が最大の音量(0db)となり、-10000が最小の音量(無音)になります。
設定値の単位は、db(デシベル)になっているため、人間が感じる音量と異なっています。人が聞いた際に自然な音量を設定するサンプルとして、volsamp.hspが用意されていますので参考にしてみてください。
※mmvol命令とは値の分解能、指定される値の意味が異なるので注意してください

%href
dmmpan
dmmload


%index
dmmpan
サウンドデータのパン設定
%group
拡張マルチメディア制御命令
%prm
p1,p2
p1(0) : サウンドID
p2(0) : パンニング値(-10000〜10000)
%inst
p1で指定したサウンドIDのパン(ステレオバランス)を設定します。
p2で設定するパンニング値を指定します。
-10000で左側100%となり、10000で右側100%になります。
※mmpan命令とは値の分解能が異なるので注意してください

%href
dmmvol
dmmload


%index
dmmloop
サウンドデータのループ設定
%group
拡張マルチメディア制御命令
%prm
p1,p2
p1(0) : サウンドID
p2(0) : ループポイント(-1=ループなし)
%inst
p1で指定したサウンドIDのループ情報を設定します。
p2に0以上の値を指定した場合には、再生が繰り返されます。
p2がマイナス値の場合は、ループを行ないません。
ストリーミング再生時は、p2でループポイントの指定を行なうことが可能です。ループポイントは、サンプル単位(44.1kHzの場合は、44100分の1秒)で数値を指定します。

%href
dmmload


%index
dmmload
サウンドデータの読み込み
%group
拡張マルチメディア制御命令
%prm
"ファイル名",p1,p2
"ファイル名" : 読み込みを行なうファイル名
p1(-1) : 登録するサウンドID (-1=自動割り当て)
p2(0)  : 再生モード(0=通常、1=リピート)
%inst
"ファイル名"で指定されたファイルをサウンドデータとして登録します。
サウンドデータは、wave形式(拡張子.wav)または、ogg vorbis形式(拡張子.ogg)のファイルを指定します。
ファイルは、memfile命令で指定したメモリ内のイメージを含め、dpmファイルなどHSPで使用可能なファイルをすべて利用することができます。

p1に登録するサウンドID番号(0から始まる任意の数値)を指定することで、これ以降はサウンドID番号をもとに各種機能を使用することができるようになります。
p1の指定を省略、またはマイナス値にした場合は、登録されていないサウンドID番号を自動的に割り当てます。
割り当てられたサウンドID番号は、dmmload命令実行後にシステム変数statに代入されます。
p2でwave形式の再生モードを指定することができます。
再生モードが、0の場合は１回のみの再生。1の場合は、繰り返し(リピート)再生になります。
ogg vorbis形式の繰り返し再生については、dmmloop命令を使用してください。

%href
dmmloop
dmmdel


%index
dmmplay
サウンドデータの再生
%group
拡張マルチメディア制御命令
%prm
p1,p2
p1(0) : サウンドID
p2(0) : 開始ポイント
%inst
p1で指定したサウンドIDを再生します。
すでに指定されたサウンドIDが再生中の場合は、最初から再生されます。
p2で開始ポイントを指定することができます。
p2が省略されるか0の場合は、サウンドデータの最初から再生され、それ以外の場合は最初からのオフセット位置として途中から再生されます。
p2で指定する単位は、サウンドデータがwav形式の場合は、バイト数(byte)。
ストリームデータ(ogg vorbis形式)の場合は、サンプル数(samples)になります。
尚、ストリームデータの同時再生には対応していません。

%href
dmmstop
dmmvol
dmmpan
dmmloop
dmmload


%index
dmmstop
サウンドデータの再生停止
%group
拡張マルチメディア制御命令
%prm
p1
p1(-1) : サウンドID (-1=すべてのサウンド)
%inst
p1で指定したサウンドIDの再生を停止します。
p1を省略するかマイナス値を指定した場合には、再生中のすべてのサウンドを停止します。

%href
dmmplay
dmmload


%index
dmmstat
サウンドデータの状態取得
%group
拡張マルチメディア制御命令
%prm
p1,p2,p3
p1 : 状態が取得される変数
p2(0) : サウンドID
p3(0) : 取得モード
%inst
p2で指定したサウンドIDの状態を取得して、p1の変数に代入します。
p3で取得するモードを指定することができます。
取得モードの値は、以下の通りです。
^p
	モード値  内容
	------------------------------------------------------
	    0     設定フラグ値
	    1     ボリューム値
	    2     パンニング値
	    3     再生レート(0=オリジナル)
	    4,5   ループポイント(下位、上位16bit)
	    16    再生中フラグ(0=停止中/1=再生中)
	   $100   oggサウンドの再生位置(実数値)
	   $101   oggサウンドの全体サイズ(実数値)
^p

%href
dmmplay
dmmload


%index
hgprm
動作パラメーターの設定
%group
拡張画面制御命令
%prm
p1,p2,p3…
p1(0)   : パラメーターID
p2(0.0) : 設定する値
%inst
HGIMG3の動作を細かく設定します。
p1で指定したパラメーターIDにp2の値を設定します。
パラメーターIDは、以下の値を指定することができます。
^p
	パラメーターID  内容
	------------------------------------------------------
	    0           地面の基点となるY座標(初期値=0.0)
	                (OBJ_LANDのオブジェクトモードで参照されます)
	    1           時間更新フラグ(初期値=1)
	                (0を指定することでアニメーションや動作の補間を停止します)
	    2           移動可能な地面の最高となるY座標(初期値=-15.0)
	                (オブジェクト自動移動の際に、指定されたY座標より上には
	                 移動することができなくなります)
	    3           ターゲットオブジェクトIDを指定
	                (この項目は現在未使用です。将来の拡張用に用意されています)
	    4           バウンド係数(初期値=1.0)
	                (OBJ_BOUNDのオブジェクトモードで参照されます)
	    5           摩擦抵抗(空中、地上)(初期値=0.95,0.95)
	                (オブジェクト自動移動の際に、加えられる抵抗を設定します
	                 このパラメーターIDは、2つの値を設定する必要があります)
^p
%href


%index
objaim
オブジェクトを目標に向けて操作する
%group
拡張画面制御命令
%prm
ObjID,Target,Mode,x,y,z
ObjID(0)   : オブジェクトID
Target(0)  : ターゲットモード(0=座標、1=角度)
Mode(0)    : モード
x(1.0)     : オプションパラメーターX
y(1.0)     : オプションパラメーターY
z(1.0)     : オプションパラメーターZ
%inst
ObjIDで指定されたオブジェクトを、目標となる値に向けて座標や角度を操作します。
Targetで、操作を行なう対象(ターゲットモード)を設定します。
0または、省略した場合には、座標を移動させることになります。
Modeで、操作の内容を選択することができます。

Mode 0 : 目標値に向けて移動(または回転)

オブジェクトのworkグループ情報に設定された座標に向けて移動(または回転)します。
(ターゲットモードが0の場合は移動、1の場合は回転角度の変更になります。)
あらかじめ、setwork命令により目標の座標を設定しておく必要があります。
オプションパラメーターX,Y,Zにより、１フレームあたりの最大移動量を設定することができます。
たとえば、(1,1,1)が指定されている場合は、X,Y,Zそれぞれ１フレームあたり最大1づつしか移動を行ないません。

Mode 1 : 目標値に向けて移動量を設定

Mode0と同様に、オブジェクトのworkグループ情報に設定された座標に向けて移動します。
ただし、実際に移動を行なうのではなく、dirグループ情報に移動量を設定します。
これは、オブジェクトのモードでOBJ_MOVEを指定している場合に、指定された座標に向けて移動させる場合などに使用するためのものです。
このモードは、常にターゲットモード0(座標)を指定するようにしてください。

Mode 4 : 目標値に向けて角度を設定

オブジェクトのworkグループ情報に設定された座標に向けて角度を設定します。
あらかじめ、setwork命令により目標の座標を設定しておく必要があります。
オプションパラメーターX,Y,Zにより、もともとのモデルが向いているベクトルを指定する必要があります。
このモードは、常にターゲットモード0(座標)を指定するようにしてください。

objaim命令は、非常に多様なオブジェクト操作が可能な反面、設定が複雑になっています。
必要な機能だけを実践しながら覚えていくといいでしょう。
命令実行後に、システム変数refdvalに対象となる座標からの距離が代入されます。

%href
event_aim


%index
event_aim
オブジェクト操作イベントを追加
%group
拡張画面制御命令
%prm
id,Mode,x,y,z
id(0)      : イベントID
Mode(0)    : モード
x(1.0)     : オプションパラメーターX
y(1.0)     : オプションパラメーターY
z(1.0)     : オプションパラメーターZ
%inst
idで指定しているイベントIDに、オブジェクト操作イベントを追加します。
objaim命令と同様の操作をイベント実行時に行なうことができます。
モード及び、オプションパラメーターX,Y,Zの値は、objaim命令と同様の内容を使用できます。
(ターゲットモードは常に0が使用されます。)

%href
objaim
newevent
setevent


%index
event_objact
アニメーション設定イベントを追加
%group
拡張画面制御命令
%prm
id,Anim,mode,speed
id(0)      : イベントID
Anim       : アニメーションインデックス値
mode(0)    : 設定モード(0=即時/1=終了時)
speed(0.0) : アニメーション再生スピード
%inst
idで指定しているイベントIDに、アニメーション設定イベントを追加します。
Anim及び、modeの設定は、objact命令と同様の値を指定することができます。
speedにより再生時の速度(objspeed命令と同様)を設定することができます。
speedの値を0.0かまたは省略にした場合は、再生速度の設定は行なわれません。

%href
objact
objspeed
newevent
setevent


%index
hgview
描画エリアの設定
%group
拡張画面制御命令
%prm
x,y,sx,sy
x(0) : 描画エリアの左上座標(X)
y(0) : 描画エリアの左上座標(Y)
sx(640) : 描画エリアの横サイズ
sy(480) : 描画エリアの縦サイズ
%inst
画面内で描画を行なうエリアを設定します。
hgviewで描画エリアを設定すると、それ以降の指定された範囲内にのみ描画されるようになります。
１つの画面内をいくつかの領域に分割して描画を行なう場合に有効です。
描画エリアを反映するためには、hgdraw命令のモードを適切な値に指定する必要がありますのでご注意ください。
hgview命令は、メインウィンドウに表示される内容を指定した範囲に収めるように描画設定を変更する命令です。メインウインドウのアスペクト比率と違うものを指定した場合は、画像が歪んで表示されることになります。
^p
例 :
	hgdraw 0			; 描画処理(全画面)
	hgview 0,0,320,240		; 描画エリアの設定
	hgdraw 1			; 描画処理(指定エリア内)
	hgsync 12			; 時間待ち
^p

%href
hgdraw


%index
texopt
文字列描画モードを設定
%group
拡張画面制御命令
%prm
space,linespace,mode
space(0)     : 文字間隔サイズ
linespace(0) : 行間隔サイズ
mode(0)      : 描画モード指定
%inst
texmes命令により描画される文字列の詳細設定を行ないます。
space,linespaceにより、表示時の文字、行間隔サイズをドット単位で微調整可能です。
0ならば通常の間隔で、数値がマイナスの場合は間隔が狭く、プラスの場合は広くなります。
またmodeにより、描画モードを以下の中から指定することができます。
^p
モード  内容
-----------------------------------------------
0       アンチエイリアスあり
1       アンチエイリアスなし
2       アンチエイリアスなし + α値のみ更新
^p

%href
texmes


%index
hgsettime
リアルタイムタイマー設定
%group
拡張画面制御命令
%prm
val,max
val(0)      : 設定する値
max(100000) : タイマーの上限値
%inst
リアルタイムタイマーの値を設定します。
リアルタイムタイマーは、1ms単位の高精度な時間を計測するもので、HGIMG3の動作中に使用可能です。
(ただし、hgsetreq命令で、hgsyncの時間待ちモード(SYSREQ_DEFTIMER)を
awaitに設定している場合はリアルタイムタイマーを利用できません。)
リアルタイムタイマーは、1msごとに常に1づつカウントアップされます。
また、タイマーの上限値を越えた場合は、0に戻ります。
リアルタイムタイマーの値は、hggettime命令によって取得することができます。

%href
hgsetreq
hggettime


%index
hggettime
リアルタイムタイマー取得
%group
拡張画面制御命令
%prm
val,type
val     : 結果が代入される変数名
type(0) : 取得タイプ
%inst
リアルタイムタイマーの情報を取得して、valで指定された変数に代入します。
リアルタイムタイマーは、1ms単位の高精度な時間を計測するもので、HGIMG3の動作中に使用可能です。
取得タイプの値によって、取得される内容が変わります。
取得タイプが0の場合は、1msごとにカウントアップされるリアルタイムタイマーの値をそのまま取得します。
取得タイプが1の場合は、リアルタイムタイマーが上限値を越えた回数を取得します。
リアルタイムタイマーの値は、いつでもhgsettime命令によって設定することができます。

%href
hgsettime



%index
meshmap
メッシュマップ情報の更新
%group
拡張画面制御命令
%prm
var,id,mode,factor
var     : メッシュマップ情報が代入されている配列変数名
id(0)   : 処理の対象となるメッシュモデルID
mode(0) : 更新モード
factor(1.0) : 更新に使用される倍率(実数)
%inst
配列変数に代入されている、メッシュマップ情報をモデルに反映させます。
メッシュマップ情報は、整数型の配列変数としてX,Y方向に分割された格子単位の情報を保持するものです。メッシュ頂点ごとの高さを始めとして、UVなど様々な情報をリアルタイムに更新することができます。
これにより、起伏のある地面やマップチップを表現することができます。
modeで指定する値により、更新される情報が変わります。
^p
モード  内容
-----------------------------------------------
0       メッシュのY座標
1       メッシュのX座標
2       メッシュのZ座標
3       メッシュのテクスチャU座標
4       メッシュのテクスチャV座標
$100    メッシュのテクスチャをマッピング
^p
modeに$100(256)を指定した場合は、メッシュマップ情報の値(0〜3)をもとに４分割された地形用のテクスチャマップチップを表示するためのUV値が設定されます。

メッシュの座標が変更された場合には、法線の再計算が行なわれます。
その場合、モードの値に16を足すことで、スムーズな法線が作成されます
(ただし、計算コストがかかります。またメッシュマップの繰り返しテクスチャを使用していないものが対象になります)。

meshmap命令は、すべての頂点に対して配列変数の内容をもとに値を設定します。
設定される値には、factorの倍率が一律に適用されます。
つまり、変数に格納されている値が15だった場合でも、factorに0.1を指定すると、実際に設定される値は1.5になります。
getvarmap命令と組み合わせて使用することで、画像データをもとにして高さや、マップチップの設定を行なうことができます。

%href
getvarmap
addmesh
objwalk



%index
getvarmap
メッシュマップ情報の作成
%group
拡張画面制御命令
%prm
var,sx,sy,mode
var     : メッシュマップ情報が代入される配列変数名
sx(0)   : X方向分割数
sy(0)   : Y方向分割数
mode(0) : 取得モード
%inst
描画バッファの内容からメッシュマップ情報を作成します。
メッシュマップ情報は、整数型の配列変数としてX,Y方向に分割された格子単位の情報を保持するものです。メッシュ頂点ごとの高さを始めとして、UVなど様々な情報を格納することができます。
描画先に指定されているHSPの画像バッファの内容を読み取って、指定された配列変数に値を設定します。
あらかじめ、代入される変数は２次元配列として初期化されている必要があります。
２次元配列のサイズは、(sx,sy)で指定された数よりもそれぞれ1づつ多い、頂点数の数だけ配列を確保する点に注意してください。
(XY分割数を(sx,sy)とすれば、配列の初期化を行なうサイズは、(sx+1,sy+1)となります。)

modeで指定する値により、取得される情報が変わります。
^p
モード  内容
-----------------------------------------------
0       白を0、黒を255とした輝度情報
1       白をスキップ、それ以外を1とする値
2       白はスキップ、それ以外を2とする値
17      白を0、それ以外を1とする値
18      白を0、それ以外を2とする値
^p
画像の点を読み取り、それに応じた値がメッシュマップ情報として代入されます。
(画像の色は関係ありません。RGBのうち、最も小さな値を読み取ります。)

%href
meshmap



%index
objwalk
メッシュマップ上の移動
%group
拡張画面制御命令
%prm
var,id,x,y,z
var      : 結果が代入される変数名
id(0)    : 移動を行なうオブジェクトID
x(0.0)   : X方向の移動量(実数)
y(0.0)   : Y方向の移動量(実数)
z(0.0)   : Z方向の移動量(実数)
%inst
指定したオブジェクトを(x,y,z)の方向に移動させます。
単に座標を移動させるだけでなく、地面を示すロッシュマップや、拡張コリジョンを設定したオブジェクトを考慮した上で、適切な移動先が設定されます。
varで指定された変数には、実際に移動した大きさ(ベクトル)が代入されます。
(変数は、double型の配列として要素0〜2に値が代入されます)

起伏のある地面の上を移動させる場合は、適切なモードがオブジェクトに対して設定されている必要があります。
OBJ_GROUNDのモードが設定されているオブジェクトは、地面として認識されます。
OBJ_STANDのモードが設定されているオブジェクトは、地面の上に配置されます。
OBJ_GRAVITYのモードが設定されているオブジェクトは、重力計算が付加されます。
また、オブジェクトを障害物として認識させる場合にも、モード及びコリジョングループの設定が必要となります。
OBJ_STATICのモードが設定されているオブジェクトは、障害物となります。

objwalk命令による移動時に得られる様々な情報をgetcolvec命令により取得することが可能です。

%href
addmesh
meshmap
objfloor
getcolvec



%index
objfloor
地面からの表示オフセット設定
%group
拡張画面制御命令
%prm
id,offset
id(0)      : 設定を行なうオブジェクトID
offset(0.0): Y方向のオフセット値(実数)
%inst
地面の上を移動する際に表示されるオブジェクトのY座標に対するオフセット値を設定します。
これは、地面にあたる座標から指定された値だけ上方向にずらして表示を行なうためのパラメーターです。
モデルの中心座標によっては、地面に埋まってしまうことがあるのを回避します。
ここで設定されたオフセット値は、objwalk命令で地面の上を移動した場合などに適用されます。

%href
regobj
objwalk



%index
getcolvec
コリジョン情報の取得
%group
拡張画面制御命令
%prm
var1,var2,var3,id,mode
var1    : Xの結果が代入される変数名
var2    : Yの結果が代入される変数名
var3    : Zの結果が代入される変数名
id(0)   : 対象となるオブジェクトID
mode(0) : 取得モード
%inst
コリジョンとオブジェクトに関する各種情報を取得します。
idで指定されたオブジェクトの情報がvar1,var2,var3で指定した変数に代入されます。
取得される情報は、modeの値によって異なります。
modeの内容は、以下の通りです。
^p
モード  内容
-----------------------------------------------
 0      足元にある地面の傾き(法線)を取得する
 1      地面のオブジェクトID、X,Y分割数を取得する
 2      コリジョンスケール値を取得する
 3      拡張コリジョンパラメーターを取得する
 4      地面のY座標、表示Yオフセット値を取得する
16      地面のオブジェクトID、X,Y分割数を取得する
17      押し出しに関する各種情報
18      オブジェクト汎用ユーザー情報を取得する
^p

モードが0〜15までは、実数値としてvar1〜var3が設定されます。
モード16以降は、整数値としてvar1〜var3が設定されます。
(あらかじめ変数の型を設定しておく必要はありません。)

モード17の押し出しに関する各種情報は、var1に押し出しを行なったかどうかのフラグ値を、var2に押し出しを行なった
オブジェクトのIDを、var3には自分が押し出された場合に押し出した側のオブジェクトIDが代入されます。

%href
modelcols
setcolscale
objwalk



%index
getnearobj
最も近くにあるオブジェクトを検索
%group
拡張画面制御命令
%prm
var,id,group,range
var      : 結果が代入される変数名
id(0)    : 検索元のオブジェクトID
group(0) : 検索対象となるコリジョングループ
range(10.0): 検索を行なう距離(実数)
%inst
検索元のオブジェクトIDから最も近い場所にある別なオブジェクトを検索します。
idで指定したオブジェクトの周囲で、最も近い位置にあるオブジェクトを検索して、varで指定された変数にオブジェクトIDを代入します。
検索の際に、groupで指定したコリジョングループと、rangeで指定した距離を参照します。
groupで指定するコリジョングループと一致するオブジェクトだけを検索します。
(あらかじめsetcoli命令等で設定されている衝突対象のグループは参照されません)
rangeは、どの距離までを検索対象とするかを指定します。
3Dオブジェクトの場合は座標の距離を、2Dオブジェクトの場合は2D座標上の距離(ドット数)を使用します。

%href
getcoli
findobj



%index
wave_init
波紋の初期化
%group
拡張画面制御命令
%prm
p1,p2
p1(32) : 初期化Xサイズ
p2(32) : 初期化Yサイズ
%inst
波紋生成機能の初期化を行ないます。
p1,p2パラメーターで、X,Y方向の分割数を指定します。
波紋生成機能は、メッシュマップの頂点に対応した波紋の伝達や弾性の計算を行なうものです。
ただし、厳密な物理演算を行なっているわけではなく、パフォーマンスを優先した簡易的なレベルに計算量が抑えられています。
これらの機能は、画面上の演出や効果などに利用することができます。
波紋生成機能を利用するためには、格子の初期化とメッシュの高さ情報を格納する配列変数を作成しておく必要があるので注意してください。
波紋の初期化は、何度でも行なうことが可能です。
初期化を行なうと、すべての頂点が高さ0にリセットされます。

%href
wave_set
wave_apply



%index
wave_set
波紋の高さを設定
%group
拡張画面制御命令
%prm
x,y,pow,mode
x(0)    : 頂点のX位置
y(0)    : 頂点のY位置
pow(0)  : 高さの値
mode(0) : 設定モード
%inst
波紋生成機能での、波紋の高さを設定します。
必ず、先にwave_init命令によって初期化を行なっておく必要があります。
頂点(x,y)の位置にある高さをpowで指定した値として設定します。
また、modeの値によりいくつかの設定方法を選択することができます。
^p
モード  内容
-----------------------------------------------
0       指定した位置のみ値を設定します
1       円錐状になるように周囲の値を設定します(弱)
2       円錐状になるように周囲の値を設定します(強)
3       球形に盛り上げます(上書き)
4       球形に盛り上げます(追加)
^p

%href
wave_init
wave_apply



%index
wave_apply
波紋の状態を取得
%group
拡張画面制御命令
%prm
var,mode
var      : 結果が代入される変数名
mode(0)  : 取得モード
%inst
波紋生成機能による計算を行ない、結果を変数に読み出します。
必ず、先にwave_init命令によって初期化を行なっておく必要があります。
また、varで指定する配列変数は、必ず整数型でグリッドサイズよりもX,Yともに1つ大きい値で初期化されている必要があります。
(頂点の数はグリッドの分割数より１つ多くなるためです)

modeの値により、処理の内容を指定することができます。
モードの内容は、以下の通りです。
^p
モード  内容
-----------------------------------------------
0       前回の結果のみを取り出す
1       弾性計算を行ない結果を取り出す
2       波紋の計算を行ない結果を取り出す
-1      変数の内容を波紋バッファに書き込む(デバッグ用)
^p
wave_apply命令は、あくまでもメッシュに適用するための計算を行なうためのものです。
実際の画面上に表示するためのメッシュマップへ適用する場合は、meshmap命令を使用する必要があります。

%href
wave_init
wave_set
meshmap


%index
delmodel
モデルの削除
%group
拡張画面制御命令
%prm
ModelID
ModelID  : モデルID
%inst
指定されたモデルを削除します。
モデルに関する情報や読み込まれているリソースもすべて解放されます。
オブジェクトに割り当てられているモデルを削除した場合には、何も表示されなくなります。(その際にもオブジェクトは削除されないので注意してください。)
モデルの削除と登録はコストの高い命令なので、毎フレームの単位で行なうような使い方は推奨されません。
必要なモデルはシーンの描画前に用意し、シーンを入れ替える場合はhgreset命令によってすべて破棄してから再登録を行なう方法が効率的です。

%href
addbox
addeprim
addmesh
addplate
addsplate
addspr
addxfile


%index
hgcnvaxis
3D座標の変換を行なう
%group
拡張画面制御命令
%prm
var_x,var_y,var_z,x,y,z,mode
var_x   : X値が代入される変数
var_y   : Y値が代入される変数
var_z   : Z値が代入される変数
x(0.0)  : 変換元のX値
y(0.0)  : 変換元のY値
z(0.0)  : 変換元のZ値
mode(0) : 変換モード
%inst
決められたモードに従って、(x,y,z)の3D座標を変換します。
結果は、var_x,var_y,var_zで指定された変数に実数型で代入されます。(変数型は自動的に設定されます)
モード値による変換の内容は以下の通りです。
^p
モード  内容
-----------------------------------------------
0       描画される2D座標(X,Y)位置+Zバッファ値
1       0と同じだが(X,Y)座標が正規化されたもの
2       ビュー変換を行なった(X,Y,Z)座標
+16     Y軸の値を正負逆として扱う
^p
モード0と1は、スクリーン上に2D投影を行なった際のX,Y座標、及びZバッファ値に変換します。
モード2では、カメラ位置を考慮したビュー変換を行なったX,Y,Z座標値に変換します。
モード値に16を加算すると、Y軸の値を正負逆として扱います。これは、HSP3.21より前のバージョンとの互換性を持たせるため用意されています。

%href
getpos



%index
hgobaq
HGIMG3でのOBAQ初期化
%group
拡張画面制御命令
%prm
p1,p2,p3
p1(0) : OBAQシステムポインタ値
p2(0) : デバッグ表示モード設定(0=ON/1=OFF)
p3(0) : 2D/3D表示モード設定(0=2D/1=3D)
%inst
HGIMG3からOBAQ(物理エンジンプラグイン)を使用するための初期化を行ないます。
あらかじめ、OBAQプラグインを初期化した上で、qgetptr命令により取得されたOBAQシステムポインタ値をp1に指定する必要があります。
p2によりデバッグ表示モードを指定します。これは、qdraw命令で指定される表示モードスイッチと同様の内容になります。
p3で、2D/3D表示の切り替えを行ないます。
デフォルトか、0が指定されている場合は2Dモードとして通常のOBAQと同様の方法でHGIMG3の画面上に描画を行ないます。
p3に1が指定されていた場合は、3DモードとなりOBAQの描画を3Dポリゴン(板)の上に貼り付ける形で3D空間上にオブジェクトとして配置することができます。
3D表示モードを利用する場合は、別途addobaq3d命令によりOBAQモデルを作成し、オブジェクトの登録を行なう必要があります。

%href
qgetptr
addobaq3d


%index
hgceldiv
HGIMG3でのセル分割指定
%group
拡張画面制御命令
%prm
p1,p2,p3,p4,p5
p1=0〜(0)  : テクスチャID
p2=1〜(0)  : 横方向の分割サイズ
p3=1〜(0)  : 縦方向の分割サイズ
p4=0〜(0)  : 横方向の中心座標
p5=0〜(0)  : 縦方向の中心座標
%inst
HGIMG3が管理するテクスチャのセル分割サイズ、及び中心座標を設定します。
これは、HGIMG3上でOBAQプラグインを使用する際に、OBAQオブジェクトのマテリアル指定でセルIDを指定する時に参照されるものです。
hgceldivは、標準命令であるceldivと同様の機能をHGIMG3で再現するためのものです。
HGIMG3単体で使用する場合は、特に設定を行なう必要はありません。

%href
celdiv


%index
addobaq3d
OBAQ3Dモデルを作成
%group
拡張画面制御命令
%prm
var
var     : 作成されたモデルIDが代入される変数名
%inst
OBAQ3Dモデルを作成します。
正常にモデルが作成されると、varで指定した変数にモデルIDが代入されます。
OBAQ3Dモデルは、OBAQプラグインが管理するオブジェクトの描画を行ないます。
OBAQ3Dモデルは、平面の板に対して描画を行ないますが、3D空間上に自由に配置し、回転をさせることができます。板(PLATE)モデルと同様の性質を持ったモデルです。
OBAQ3Dモデルは、必ずhgobaq命令によりOBAQの初期化と3D表示の設定を行なっておく必要があります。

%href
hgobaq


%index
addline
線分モデルを作成
%group
拡張画面制御命令
%prm
var,p1,p2,p3
var     : 作成されたモデルIDが代入される変数名
p1(-1)  : テクスチャID
p2(0)   : セルID、カラーコード
p3(16)  : 線サイズ
%inst
線分モデルを作成します。
正常にモデルが作成されると、varで指定した変数にモデルIDが代入されます。
線分モデルは、オブジェクトに設定された座標から、オブジェクトのworkグループに設定された座標(相対位置)に線を引く機能を持っています。
p1にテクスチャIDを指定した場合は、テクスチャを使用して線を描画します。
その際には、p2で指定した値をセルID(hgceldivで分割された中でのID)として扱います。
p1がデフォルトかマイナス値の場合は、p2で指定されたカラーコードで線を描画します。また、テクスチャ描画時はp3の線サイズが有効になります。

%href
hgceldiv


%index
newemit
エミッターを作成
%group
拡張画面制御命令
%prm
p1,p2,p3,p4
p1     : 作成されたエミッターIDが代入される変数名
p2(0)  : エミッターモード
p3(8)  : オブジェクト生成数
p4(-1) : エミッターID
%inst
エミッターを初期化し、p1で指定した変数にエミッターIDを代入します。
^
エミッターとは、複数のオブジェクトを一度に発生させることのできる特別な仕組みです。
ランダムなパラメーターなど自由な組み合わせで生成することのできる、オブジェクト発生器と考えてください。
新しくエミッターを作成する場合には、必ずnewemit命令でエミッターIDを取得しておく必要があります。
p2で初期化するエミッターのモードを指定することができます。
^p
モード          値      内容
-----------------------------------------------
EMITMODE_NONE	0	何もしない
EMITMODE_STATIC	1	出現のみで移動しない
EMITMODE_CIRCLE	2	同心円状に移動
EMITMODE_RANDOM	3	ランダムな方向に移動
EMITMODE_LOOKAT	16	移動方向に向ける(他と併用可能)
^p
p3で一度に生成されるオブジェクトの数を設定します。
デフォルトでは、8個のオブジェクトが一度に生成される設定になります。
作成されたエミッターには、「emit_」で始まるエミッター設定命令によって多彩な動作を登録しておくことができます。
p4で上書きするエミッターIDを指定することができます。
新規に作成する場合は、省略するかマイナス値にしておいてください。
^
エミッターが作成されてすぐにオブジェクトが発生するわけではありません。
あらかじめ、発生条件を色々と設定した上で必要な時に発生させます。
エミッターを実際に利用する場合は、hgemit命令によっていつでも発生させることができるほか、setobjemit命令によりオブジェクトに対して適用することも可能です。

%href
delemit
hgemit
setobjemit


%index
delemit
エミッターを削除
%group
拡張画面制御命令
%prm
p1
p1(0)   : エミッターID
%inst
p1で指定されたエミッターを削除します。
削除されたエミッターIDは、再度newemit命令で初期化するまでは使用できません。
%href
newemit


%index
emit_size
エミッターの発生範囲を設定
%group
拡張画面制御命令
%prm
id,dx,dy,dz
id(0)   : エミッターID
dx(0.0) : 発生範囲X(実数)
dy(0.0) : 発生範囲Y(実数)
dz(0.0) : 発生範囲Z(実数)
%inst
idで指定されたエミッターが発生するオブジェクトの範囲を設定します。
発生範囲は、もともと指定された発生座標からどの程度までちらばるかを指定するものです。
たとえば、エミッターの発生座標が(10,10,10)であったとして、発生範囲が(20,10,0)と設定された場合には、
エミッターから発生されるオブジェクトは、(10〜30,10〜20,10)の範囲内で乱数によりちらばることになります。
これにより、発生する初期位置をある程度ばらつきを持たせることができます。
emit_sizeの設定が行なわれないエミッターは、初期値として(0,0,0)が設定されています。

%href
newemit


%index
emit_speed
エミッターのスピード設定
%group
拡張画面制御命令
%prm
id,speed,spdopt
id(0)       : エミッターID
speed(1.0)  : スピード初期値(実数)
spdopt(0.0) : スピード範囲(実数)
%inst
idで指定されたエミッターが発生するオブジェクトの速度を設定します。
オブジェクトの速度は、発生すると同時に移動を開始するオブジェクトに与えられる数値です。
スピードの値が大きいほど、速い速度で移動を行ないます。
speedパラメーターで、設定するスピードの初期値を指定します。
spdoptが0より大きい値の場合は、その範囲内でばらつきを持たせます。
たとえば、(1.0,5.0)が指定された場合には、速度は1.0〜6.0の範囲内で乱数によりちらばることになります。
emit_speedの設定が行なわれないエミッターは、初期値として(1.0,0.0)が設定されています。
%href
newemit


%index
emit_angmul
エミッターの角度係数を設定
%group
拡張画面制御命令
%prm
id,dx,dy,dz
id(0)   : エミッターID
dx(0.0) : 角度係数X(実数)
dy(0.0) : 角度係数Y(実数)
dz(0.0) : 角度係数Z(実数)
%inst
idで指定されたエミッターが発生する際のオブジェクト移動角度係数を設定します。
エミッターによりオブジェクトが発生する場合に、移動する方向の角度が算出されます。
移動方向は、基本的にEMITMODE_RANDOMモードの場合は、全周360度からランダムに選ばれ、EMITMODE_CIRCLEモードの場合は全周360度を発生する個数で均等に割り付けます。
角度係数は、この角度に乗算される数値になります。
これにより、全周360度だけでなく180度、90度といった限定された範囲内での移動を設定することができます。
たとえば、角度係数に0.5を指定した場合は、全周の半分(180度)で計算が行なわれます。さらに、emit_angopt命令により角度加算値を設定することにより移動の範囲を自由に設定することが可能です。
角度係数は、X,Y,Z軸ごとに個別設定可能です。また、2Dスプライトの場合はZ軸回転のみが反映されます。
emit_angmulの設定が行なわれないエミッターは、初期値として(1,1,1)が設定されています。

%href
newemit
emit_angopt


%index
emit_angopt
エミッターの角度加算値を設定
%group
拡張画面制御命令
%prm
id,dx,dy,dz
id(0)   : エミッターID
dx(0.0) : 角度加算値X(実数)
dy(0.0) : 角度加算値Y(実数)
dz(0.0) : 角度加算値Z(実数)
%inst
idで指定されたエミッターが発生する際のオブジェクト移動角度加算値を設定します。
エミッターによりオブジェクトが発生する場合に、移動する方向の角度が算出されますが、その値に加算する定数をX,Y,Z軸ごとに指定できます。
発生するオブジェクトの移動方向(角度)は、
^p
	オブジェクト角度 = 基本角度 * 角度係数 + 角度加算値
^p
という形で算出されます。(基本角度の詳細は、emit_angmul命令のリファレンスを参照してください)
角度係数を設定するemit_angmul命令と併用することで、移動角度の範囲を限定した形でオブジェクトを発生させることが可能になります。
emit_angoptの設定が行なわれないエミッターは、初期値として(0,0,0)が設定されています。

%href
newemit
emit_angmul


%index
emit_model
エミッターの発生モデルを設定
%group
拡張画面制御命令
%prm
id,model,modelnum,objmode,efx
id(0)       : エミッターID
model(0)    : モデルID
modelnum(0) : モデルID範囲
objmode(0)  : オブジェクトモード設定値
efx(0x100)  : efx設定値

%inst
idで指定されたエミッターが発生するオブジェクトのモデルIDなど詳細パラメーターを設定します。
エミッターから発生されるオブジェクトの種類やモード設定など重要な要素を指定します。
エミッターを使用する際には必ず設定するようにしてください。
modelパラメーターで、発生するモデルIDを指定します。モデルIDは、regobj命令で設定可能なモデルIDと同様です。
modelnumで、発生するモデルIDの範囲を指定することができます。
これは、ランダムに別々なモデルを発生させる際に使用することができます。
たとえば、モデルIDが3で、modelnumが4の場合は、モデルID3〜6の範囲でランダムにオブジェクトが発生することになります。
単一のモデルだけを発生させる場合は、modelnumは0のままにしておいてください。
objmodeパラメーターで、発生時に設定されるオブジェクトのモードを指定することができます。モード値は、regobj命令で指定する値と同様です。
efxパラメーターで、発生時に設定されるオブジェクトのefxパラメーターを指定することができます。
efxパラメーターは、setefx命令で指定する１番目のパラメーターと同様です。
これにより、半透明や加算などの特殊効果設定をオブジェクトに適用することが可能になります。

%href
newemit
regobj


%index
emit_event
エミッターのイベント設定
%group
拡張画面制御命令
%prm
id,event,eventnum
id(0)   : エミッターID
event(0)    : イベントID
eventnum(0) : イベントID範囲
%inst
idで指定されたエミッターが発生するオブジェクトのイベントIDを設定します。
eventパラメーターで、発生するイベントIDを指定します。イベントIDは、setevent命令で設定可能なイベントIDと同様です。
eventnumで、発生するイベントIDの範囲を指定することができます。
これは、ランダムに別々なイベントを発生させる際に使用することができます。
たとえば、イベントIDが2で、eventnumが5の場合は、モデルID2〜6の範囲でランダムにイベントが発生することになります。
単一のイベントだけを発生させる場合は、eventnumは0のままにしておいてください。
emit_eventの設定が行なわれないエミッターは、オブジェクト発生時にイベント設定は行なわれません。

%href
newemit
setevent


%index
emit_num
エミッターの発生数設定
%group
拡張画面制御命令
%prm
id,num,numopt
id(0)     : エミッターID
num(0)    : オブジェクト発生数
numopt(0) : 発生範囲
%inst
idで指定されたエミッターが発生するオブジェクトの数を設定します。
numで、発生するオブジェクトの数を指定することができます。
これは、ランダムに別々な数のオブジェクトを発生させる際に使用することができます。
たとえば、オブジェクト発生数が5で、numoptが10の場合は、5〜14の範囲でランダムに発生数が変わることになります。
emit_numの設定が行なわれないエミッターは、newemitで指定された発生個数とともに、発生範囲(numopt)は0になっています。

%href
newemit


%index
emit_group
エミッターのグループ設定
%group
拡張画面制御命令
%prm
id,mygroup,enegroup
id(0)       : エミッターID
mygroup(0)  : 自分が属するグループ値
enegroup(0) : 衝突を検出する対象となるグループ値
%inst
idで指定されたエミッターが発生するオブジェクトのコリジョングループを設定します。これは、setcoli命令によりオブジェクトに設定する値と同様です。
特定のコリジョングループを持つオブジェクトを発生させる場合に使用してください。
emit_groupの設定が行なわれないエミッターは、オブジェクト発生時にコリジョングループ設定は行なわれません。

%href
newemit
setcoli


%index
hgemit
エミッター発生実行
%group
拡張画面制御命令
%prm
id,dx,dy,dz
id(0)   : エミッターID
dx(0.0) : X座標値(実数)
dy(0.0) : Y座標値(実数)
dz(0.0) : Z座標値(実数)
%inst
idで指定されたエミッターを、(dx,dy,dz)の座標から発生させます。
あらかじめエミッターから発生させるオブジェクトの設定を行なっておく必要があります。
実行後に、発生させたオブジェクトの合計数がシステム変数statに代入されます。
配置されているオブジェクトに対してエミッターを適用する場合は、setobjemit命令を使用してください。
%href
newemit
setobjemit


%index
setobjemit
オブジェクトにエミッター適用
%group
拡張画面制御命令
%prm
p1,p2
p1(0) : オブジェクトID
p2(0) : エミッターID
%inst
配置されているオブジェクトをエミッターの発生元として設定します。
p1で指定されたオブジェクトに、p2のエミッターを割り当てます。
以降は、オブジェクト表示時にエミッターとしても動作するようになります。
エミッターIDにマイナス値を指定することで、エミッターの適用を解除することができます。
%href
hgemit


%index
groupmod
同一グループに演算適用
%group
拡張画面制御命令
%prm
group,mocid,dx,dy,dz,opt
group(0) : 演算の対象となるコリジョングループ
mocid(0) : MOCの適用先
dx(0.0)  : X演算値(実数)
dy(0.0)  : Y演算値(実数)
dz(0.0)  : Z演算値(実数)
opt(0)   : 演算方法
%inst
同じコリジョングループに属しているモデルに対してまとめて演算を行ないます。
有効なすべてのオブジェクトの中で、groupパラメーターで指定したコリジョングループが指定されているものが操作の対象となります。
groupパラメーターで指定されるコリジョングループには、複数のビットを組み合わせることが可能です。たとえば、グループ1とグループ2を同時に指定する場合は、3という値になります。
mocidパラメーターは、MOCグループを示す値を指定します。MOCグループには、以下を指定することが可能です。
^p
MOCグループ    mocid
---------------------------
  pos           0
  ang           1
  scale         2
  dir           3
  efx           4
  work          5
^p
(dx,dy,dz)により演算で使用する値をします。
演算の方法は、optパラメーターによって指定されまます。optパラメーターには、以下を指定することができます。
^p
  opt  演算方法
---------------------------
   0   代入
   1   加算
   2   減算
   3   乗算
^p
optが省略、または0が指定された時は、(dx,dy,dz)の値がそのまま対象となるオブジェクトに代入されます。
それ以外の値では、すでに代入されている(X,Y,Z)値に対して(dx,dy,dz)が演算されます。
%href
setcoli


%index
hgqview
HGIMG3上のOBAQビュー設定
%group
拡張画面制御命令
%prm
ofsx,ofsy,ofsz,zoom
ofsx(0.0)  : X方向の表示オフセット(実数)
ofsy(0.0)  : Y方向の表示オフセット(実数)
ofsz(0.0)  : Z方向の表示オフセット(実数)
zoom(4.0) : ズーム倍率(実数)
%inst
HGIMG3上でobaqオブジェクトを表示する際の設定を行ないます。
zoomパラメーターで、HGIMG3表示時のズーム倍率を指定します。
(ofsx,ofsy,ofsz)でX,Y,Z軸に対する表示オフセットを指定することができます。
%href
qview



%index
hgqcnvaxis
OBAQ座標をHGIMG3座標に変換
%group
拡張画面制御命令
%prm
x,y,z,srcx,srcy
(x,y,z)    : 取得する変数
srcx(0.0)  : OBAQ上のX座標(実数)
srcy(0.0)  : OBAQ上のY座標(実数)
%inst
OBAQ上のX,Y座標をHGIMG3上のX,Y,Z座標に変換します。
これは、OBAQオブジェクトを3Dオブジェクトとして表示する際の座標を求めるものです。
(srcx,srcy)でOBAQ上のX,Y座標を指定すると、(x,y,z)に指定された変数に実数型でHGIMG3上のX,Y,Z座標が代入されます。
%href
qcnvaxis


%index
enumobj
オブジェクトの検索開始
%group
拡張画面制御命令
%prm
group
group    : コリジョングループ
%inst
指定したコリジョングループに属するオブジェクトを検索します。
検索結果は、getenum命令により取得することができます。
検索結果が複数ある場合は、getenum命令を呼び出すたびに次の結果が返されます。
すべての検索が終了した場合は、getenum命令でオブジェクトのIDとして-1が返されます。
%href
getenum


%index
getenum
オブジェクトの検索結果を取得
%group
拡張画面制御命令
%prm
var
var      : 結果が代入される変数名
%inst
enumobj命令により検索されたオブジェクトのIDを取得します。
getenum命令を実行するたびに、次の検索結果が取得されます。取得が終わった場合には、-1が代入されます。

%href
enumobj



