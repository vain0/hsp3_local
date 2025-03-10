;
;	HSP help manager用 HELPソースファイル
;	(先頭が「;」の行はコメントとして処理されます)
;

%type
拡張命令
%ver
3.4
%note
hspext.asをインクルードすること。
%dll
hspext
%date
2009/08/01
%author
onitama
%url
http://hsp.tv/
%port
Win



%index
aplsel
任意のウィンドウ捕獲
%group
OSシステム制御命令
%prm
"window name",p1
"window name" : 取得するウィンドウのタイトル名
p1=0〜(0)     : 開始ID

%inst
指定したウィンドウを捕獲して、メッセージを送信する準備をします。
^
"window name" に、ウィンドウのタイトルに表示されている文字列を指定することで、ウィンドウを検索します。
"window name"は、終わりの文字列を省略することが可能です。
たとえば、"ＨＳＰスクリプトエディタ"という名前のウィンドウは、"ＨＳＰ"だけでも、"ＨＳＰスクリ"という文字列でもマッチします。
ただし、同じ名前のタイトルがあった場合には、どのウィンドウがマッチするかは不明になります。
^
p1で開始IDを指定することで検索を開始する検索IDを決めることができます。
この検索IDは、 すべてのウィンドウ (およびタスク) のID番号で、0から順番に値がつけられています。
「aplsel "",p1」と記述すると、検索IDがp1のウィンドウを捕獲することができます。
^
aplsel命令が実行されて、ウィンドウの検索に成功した場合は、システム変数statに0が代入されて、システム変数 refstrに正確なウィンドウタイトル名が代入されます。
ウィンドウの検索に失敗した場合は、システム変数 statに1が代入されメッセージの送信を行なうことはできません。

%href
aplobj
aplact
aplfocus
aplstr
aplkey
aplkeyd
aplkeyu
aplget
apledit


%index
aplobj
任意のオブジェクト捕獲
%group
OSシステム制御命令
%prm
"object name",p1
"object name" : 取得するオブジェクトのクラス名
p1=0〜(0)     : 開始オブジェクトID

%inst
aplsel命令で捕獲したウィンドウに属するオブジエクト(コントロール)を捕獲して、メッセージを送信する準備をします。
^
"object name" に、オブジェクト(コントロール)のクラス名を指定することで、オブジェクトを検索します。
^
p1で、開始IDを指定することで検索を開始する検索オブジェクトIDを決めることができます。このIDは、0から順番に値がつけられています。
「aplsel "",p1」と記述すると、オブジェクトIDを捕獲することができます。
^
aplobj命令が実行されて、オブジェクトの検索に成功した場合は、システム変数 statに0が代入されて、システム変数refstrに正確なオブジェクトのクラス名が代入されます。
ウィンドウの検索に失敗した場合は、システム変数statに1が代入され、メッセージの送信を行なうことはできません。

%href
aplsel
aplact
aplfocus
aplstr
aplkey
aplkeyd
aplkeyu
aplget
apledit


%index
aplact
ウィンドウをアクティブにする
%group
OSシステム制御命令

%inst
aplsel命令、およびaplobj命令で捕獲したウィンドウをアクティブにして、キーボード入力が可能な状態にします。
^
aplkey命令などで、 HSP以外のウィンドウにキーを送信する場合には、aplact命令で、ウィンドウをアクティブにしておく必要があります。

%href
aplsel
aplobj
aplfocus
aplstr
aplkey
aplkeyd
aplkeyu
aplget
apledit


%index
aplfocus
キー送信先をデフォルトにする
%group
OSシステム制御命令

%inst
キー送信の対象となるウィンドウをデフォルトに戻します。
^
デフォルト設定では、現在アクティブなウィンドウの、キーボード入力フォーカスがあるオブジェクトに対してキー送信されます。

%href
aplsel
aplobj
aplact
aplstr
aplkey
aplkeyd
aplkeyu
aplget
apledit


%index
aplstr
文字列をキー送信
%group
OSシステム制御命令
%prm
"strings"
"strings" : キー送信する文字列

%inst
"strings" で指定した文字列を、キーボード入力データとして、捕獲先のオブジェクトに送信します。
^
aplstr命令は文字列をそのまま送信することができますが、 [ALT]キーなどの特殊キーの情報は送信できません。
aplstr命令は、キーの情報をキューにためるだけです。
実際に押した結果を反映するためには、waitや await命令を後に入れる必要があります。
一度に大量の情報を送りすぎると、キューがあふれてうまく送信されなくなる可能性があります。
aplstr命令は、すべてのアプリケーションにおいて、認識されるとは限りません。aplstr命令の送信を受け付けない場合は、aplkey命令などで試してみてください。

%href
aplsel
aplobj
aplact
aplfocus
aplkey
aplkeyd
aplkeyu
aplget
apledit


%index
aplkey
キーコード送信
%group
OSシステム制御命令
%prm
p1,p2
p1=0〜(0) : キーコード
p2=0〜(0) : 特殊キーコード
^p
( 1 = SHIFT / 2 = CTRL / 4 = ALT )
^p

%inst
キーを押した情報を捕獲先のオブジェクトに送信します。
^
p1で、キーコードを指定します。これは、getkey命令で使用しているキーコードと同一のものです。(下の表を参照)
^
p2で特殊キーの指定をします。 1ならば、シフトキー、2ならばCTRLキー、4ならばALTキーが同時に押されていることになります。
^
aplkey命令は、キーの情報をキューにためるだけです。
実際に押した結果を反映するためには、waitやawait命令を直後に入れる必要があります。
p1に0を指定した場合には、特殊キーコードの情報のみが送信されます。
^p
     キーコード一覧
 ---------------------------------------------
        3 : キャンセル（[CTRL]+[BREAK]）
        4 : ３ボタンマウスのまん中のボタン
        8 : [BACKSPACE]（PC98の[BS]）
        9 : [TAB]
       13 : [ENTER]
       16 : [SHIFT]
       17 : [CTRL]
       18 : [ALT]（PC98の[GRPH]）
       20 : [CAPSLOCK]
       27 : [ESC]
       32 : スペースキー
       33 : [PAGEUP]（PC98の[ROLLDOWN]）
       34 : [PAGEDOWN]（PC98の[ROLLUP]）
       35 : [END]（PC98の[HELP]）
       36 : [HOME]（PC98の[HOMECLR]）
       37 : カーソルキー[←]
       38 : カーソルキー[↑]
       39 : カーソルキー[→]
       40 : カーソルキー[↓]
 ---------------------------------------------
  48〜57  : [0]〜[9]（メインキーボード）
  65〜90  : [A]〜[Z]
  96〜105 : [0]〜[9]（テンキー）
 112〜121 : ファンクションキー [F1]〜[F10]
 ---------------------------------------------
^p

%href
aplsel
aplobj
aplact
aplfocus
aplstr
aplkeyd
aplkeyu
aplget
apledit


%index
aplkeyd
キー押し下げ送信
%group
OSシステム制御命令
%prm
p1,p2
p1=0〜(0) : キーコード
p2=0〜(0) : 特殊キーコード
^p
( 1 = SHIFT / 2 = CTRL / 4 = ALT )
^p

%inst
キーを押した情報を捕獲先のオブジェクトに送信します。
^
p1でキーコードを指定します。これは、getkey命令で使用しているキーコードと同一のものです。
^
p2で特殊キーの指定をします。1ならば、シフトキー、2ならばCTRLキー、 4ならばALTキーが同時に押されていることになります。
^
aplkey命令とは違い、aplkeyd命令は、キーを押した情報だけを送信します。
キーを押しつづけたい場合や、当時に複数のキーを押した状態を作り出したい時に使用してください。
また、 aplkeyd命令でキーを押した情報を送信した後は、必ずキーを離したという情報をaplkeyu命令で送信しなければなりません。aplkeyd命令は、キーの情報をキューにためるだけです。
実際に押した結果を反映するためには、waitや await命令を直後に入れる必要があります。
p1に0を指定した場合には、特殊キーコードの情報のみが送信されます。

%href
aplsel
aplobj
aplact
aplfocus
aplstr
aplkey
aplkeyu
aplget
apledit


%index
aplkeyu
キー押し上げ送信
%group
OSシステム制御命令
%prm
p1,p2
p1=0〜(0) : キーコード
p2=0〜(0) : 特殊キーコード
^p
( 1 = SHIFT / 2 = CTRL / 4 = ALT )
^p

%inst
キーを離した情報を捕獲先のオブジェクトに送信します。
^
p1でキーコードを指定します。これは、getkey命令で使用しているキーコードと同一のものです。
^
p2で特殊キーの指定をします。1ならば、シフトキー、2ならばCTRLキー、 4ならばALTキーが同時に押されていることになります。
^
aplkeyu命令は、キーを離した情報だけを送信します。必ず、aplkeyd命令と対にして使用してください。
p1の指定を省略すると、 aplkeyd命令で指定したキーコードと、特殊キーコードと同じものが使われます。
aplkeyu命令は、キーの情報をキューにためるだけです。
実際に押した結果を反映するためには、waitや await命令を直後に入れる必要があります。

%href
aplsel
aplobj
aplact
aplfocus
aplstr
aplkey
aplkeyd
aplget
apledit


%index
aplget
オブジェクトの文字列を取得
%group
OSシステム制御命令
%prm
p1,p2
p1=変数    : オブジェクトの文字列を読み出す変数名
p2=0〜(64) : 最大文字数

%inst
aplobj命令で選択されたオブジェクトに設定された文字列を取得します。
^
オブジェクトが "BUTTON" などの場合はそこに設定されている文字列が、またウィンドウの場合はタイトルに表示されている文字列を読み出します。
^
p2が省略されている場合は最大64文字まで読み出します。
64文字以上の文字列を読み出す場合は、p2に最大文字数を指定してください。
(あらかじめ読み出す変数に十分なバッファが確保されている必要があります)
^
aplfocus命令で、ウィンドウの指定がデフォルトになっている場合は現在アクティブなウィンドウのタイトル文字列が読み出されます。
aplget命令で "EDIT" オブジェクトの編集内容を呼び出すことはできません。
"EDIT"オブジェクトの編集内容以外の情報は、 apledit命令で取得することが可能です。

%href
aplsel
aplobj
aplact
aplfocus
aplstr
aplkey
aplkeyd
aplkeyu
apledit


%index
apledit
エディットコントロール情報取得
%group
OSシステム制御命令
%prm
p1,p2,p3
p1=変数 : 情報が格納される数値型変数名
p2=0〜2 : 取得情報No.
p3=0〜  : 行インデックス指定

%inst
aplobj命令で選択された"EDIT"タイプのオブジェクトの情報を取得します。
^
p1に読み出す先の変数名を指定して、p2で情報の種類を指定します。
^
p2=0 : 現在のカーソル位置(1byte単位)
p2=1 : 全体の行数
p2=2 : p3で指定した行にある文字数
^
パラメータp3は、p2に2が指定された時のみ意味を持ちます。
また、p2が0の時 (カーソル位置取得)はp1で指定した変数に、先頭からカーソルまでのバイト数、もし選択範囲があれば、システム変数statにカーソル位置から選択されている範囲のバイト数が代入されます。

%href
aplsel
aplobj
aplact
aplfocus
aplstr
aplkey
aplkeyd
aplkeyu
aplget



%index
clipset
クリップボードテキスト転送
%group
OSシステム制御命令
%prm
"strings"
"strings" : クリップボードに送る文字列

%inst
"strings"で指定した文字列を、クリップボードに送ります。
^
クリップボードに送られたテキストデータは、他のアプリケーションで貼り付け(ペースト)が可能になります。


%index
clipsetg
クリップボード画像転送
%group
OSシステム制御命令

%inst
現在選択されている画面をビットマップデータとして、クリップボードに送ります。
^
クリップボードに送られた画像データは、 他のアプリケーションで 貼り付け(ペースト)が可能になります。


%index
clipget
クリップボードテキスト取得
%group
OSシステム制御命令
%prm
p1,p2
p1=変数    : 情報が格納される変数名
p2=0〜(64) : 最大文字数

%inst
クリップボードに送られているテキストの内容を読み出し、p1で指定した文字列型変数に代入します。

読み出す内容は他のアプリケーションなどでコピーされたテキストのみです。
画像や音声は取り出すことができません。p2が省略されている場合は最大64文字まで読み出します。64文字以上の文字列を読み出す場合は、p2に最大文字数を指定してください。
(あらかじめ読み出す変数に十分なバッファが確保されている必要があります)



%index
comopen
シリアルポートを初期化
%group
通信制御命令
%prm
p1,"protocol"
p1=0〜(0)  : COMポート番号
"protocol" : プロトコル指定文字列

%inst
シリアルポートを初期化して送受信を可能な状態にします。
^
シリアル通信命令セットを使う場合には、最初に comopen命令でポートの初期化をする必要があります。
p1で、COMポートの番号を指定します。 1ならばCOM1、2ならばCOM2…が対応します。 p1が0の場合は、シリアルではなくプリンタ(パラレル)ポートが指定されます。
"protocol"で、プロトコル指定文字列を記述することができます。
プロトコル指定文字列は以下のような記述をする必要があります。
^
"baud=1200 parity=N data=8 stop=1"
^
上の例では、ボーレートが1200bps、パリティビットなし、データビット8、ストップビット1という指定になります。
"protocol"に何も書かなかった場合(""を指定)は、現在のデフォルト値が使用されます。通常は、何も指定しなくても問題ありません。
^
シリアル通信命令では、同時に１つのポートまでしか制御できません。
複数のシリアルポートを同時に監視するソフトを構築することはできませんのでご注意ください。
^
シリアルポートは、一度初期化されると、開放するまで他のアプリケーションが使用できなくなってしまうので、必ず最後にポートを開放することを忘れないでください。特に通信中にクローズボックスを押して終了してしまうことのないようにonexit命令などで、終了時に注意を促すようなスクリプトを作成してください。
^
シリアルポートの初期化に成功すると、システム変数 statの値が0になります
。もし、初期化に失敗した場合はシステム変数statの値は1になります。

%href
comclose
comput
comget
computc
comgetc
comstat
comcontrol


%index
comclose
シリアルポートを解放
%group
通信制御命令

%inst
シリアルポートとの通信を終了します。
^
comopen命令で初期化されたシリアルポートは、必ず comclose命令で最後に解放する必要があります。
comclose命令でポートが解放されないままプログラムが終了すると、他のプログラムがポートに二度とアクセスできなくなってしまうので注意してください。
。

%href
comopen


%index
comput
シリアルポートに送信
%group
通信制御命令
%prm
"send-string"
"send-string" : 送出文字列

%inst
"send-string"で指定された文字列をシリアルポートに送出します。
^
命令の実行後、システム変数statに結果が返されます。
システム変数 statが0ならば通信に失敗し送信されていないことを示します。
1以上の場合は、送信に成功したバイト数(文字数)が代入されています。

%href
comopen
comclose
computc


%index
computc
シリアルポートに送信
%group
通信制御命令
%prm
p1
p1=0〜255(0) : 送信データ(1バイト)

%inst
p1で指定された1バイトの数値データをシリアルポートに送出します。
^
命令の実行後、システム変数statに結果が返されます。
システム変数 statが0ならば通信に失敗し送信されていないことを示します。

%href
comopen
comclose
comput


%index
comgetc
シリアルポートから受信
%group
通信制御命令
%prm
p1
p1=数値型変数 : 受信データが代入される変数名

%inst
シリアルポートから1バイト受信し、p1で指定した変数に代入します。
^
命令の実行後、システム変数statに結果が返されます。
システム変数 statが0ならば、受信バッファが空になっていることを示しています。

%href
comopen
comclose
comget


%index
comget
シリアルポートから文字列を受信
%group
通信制御命令
%prm
p1,p2
p1=文字列型変数 : 受信データが代入される変数名
p2=1〜(64)      : 受信文字数指定

%inst
シリアルポートから文字列を受信し、p1で指定した変数に代入します。
^
p2で、受信する文字数を指定することができます。命令の実行後、システム変数statに結果が返されます。
システム変数 statが0ならば、受信バッファが空になっていることを示しています。
受信に成功すると、p1に文字列が代入され、システム変数statに実際に受信した文字数が代入されます。

%href
comopen
comclose
comgetc



%index
comcontrol
シリアルポートの特殊コントロール
%group
通信制御命令
%prm
p1
p1=1〜9(0)      : 特殊コントロール機能指定

%inst
シリアルポートに対して特殊なコントロールを行ないます。
p1で、コントロールする機能を以下の中から指定します。
^p
           値      機能
-----------------------------------------------------------------------
SETXOFF    1       XOFF 文字を受信したときのように送信を行います。
SETXON     2       XON 文字を受信したときのように送信を行います。
SETRTS     3       RTS（ 送信要求）信号を送信します。
CLRRTS     4       RTS（ 送信要求）信号を消去します。
SETDTR     5       DTR（ データ端末準備完了）信号を送信します。
CLRDTR     6       DTR（ データ端末準備完了）信号を消去します。
RESETDEV   7       デバイスをリセットします
SETBREAK   8       文字送信を中断し、送信回線を切断状態にします。
CLRBREAK   9       送信回線の切断状態を解除して、文字送信を再開します。
^p
状態取得が正常に行なわれた場合にはシステム変数statが0になります。
エラーが発生した場合には、システム変数statは1になります。

%href
comstat



%index
comstat
シリアルポートの状態取得
%group
通信制御命令
%prm
p1
p1=数値型配列変数 : 状態データが代入される変数名

%inst
シリアルポートの状態を取得してp1で指定した変数に代入します。
p1で指定する変数は、配列でp1.0〜p1.3までに以下の情報が代入されます。
^p
-----------------------------------------------------------------------
a.0
	エラー発生時に以下のエラーフラグの組み合わせが代入されます。

	CE_RXOVER           0x0001  受信バッファのオーバーフロー。 
	CE_OVERRUN          0x0002  オーバーランエラー。 
	CE_RXPARITY         0x0004  受信時のパリティエラー。 
	CE_FRAME            0x0008  フレーミングエラー。 
	CE_BREAK            0x0010  ブレーク状態。 
	CE_TXFULL           0x0100  送信バッファがいっぱいになりました。 
	CE_PTO              0x0200  タイムアウト。
	CE_IOE              0x0400  一般 I/O エラー。 
	CE_DNS              0x0800  デバイスが選択されていません。
	CE_OOP              0x1000  給紙切れエラー。
	CE_MODE             0x8000  サポート外のモードです。

a.1
	以下のデバイス状態フラグが代入されます。

	bit0 fCtsHold  ON: 送信は CTS 待ち  
	bit1 fDsrHold  ON: 送信は DSR 待ち  
	bit2 fRlsdHold  ON: 送信は RLSD (CD) 待ち  
	bit3 fXoffHold  ON: XOFF を送信したため 送信は停止中  
	bit4 fXoffSent  ON: XOFF を送信したため 送信は停止中  
	bit5 fEof  ON: EOF を受信した  
	bit6 fTxim  ON: 送信バッファにデータが残っている  

a.2
	cbInQue  受信バッファにあるデータのバイト数  
a.3
	cbOutQue  送信バッファにあるデータのバイト数  
-----------------------------------------------------------------------
^p
状態取得が正常に行なわれた場合にはシステム変数statが0になります。
エラーが発生した場合には、システム変数statは1になります。


%href
comcontrol





%index
gfini
対象画面を設定
%group
拡張画面制御命令
%prm
p1,p2
p1=0〜(0) : 画面操作を行なうXサイズ(dot単位)
p2=0〜(0) : 画面操作を行なうYサイズ(dot単位)

%inst
フルカラー画面操作命令を行なう対象の画面を設定します。
^
必ず gfini命令で対象の画面を設定してから、他のフルカラー画面操作命令を使用してください。
gfini命令では、現在 gsel命令で選択されているウィンドウのカレントポジションから、p1,p2で指定したサイズを描画対象とします。
必ずフルカラーモードで初期設定をした画面に対して行なってください。


%index
gfcopy
半透明コピー
%group
拡張画面制御命令
%prm
p1
p1=0〜100(0) : 半透明コピーレート(%)

%inst
p1で指定したレートで画面イメージをコピーします。
^
コピー先は、gfini命令で設定した画面になります。
コピー元は、gfcopy命令が実行された時点に、描画先に設定されている画面のカレントポジションになります。
p1のレートは 0〜100(%)で、100%は普通のコピーになります。この命令を実行しただけでは実際の画面は更新されません。
「redraw 1」などで画面の更新を行なって初めて反映されます。

%href
gfini


%index
gfdec
画面の色減算
%group
拡張画面制御命令
%prm
p1,p2,p3
p1,p2,p3=0〜255(0) : 色コード（R,G,Bの輝度)

%inst
gfini命令で設定した画面内のすべてのドットに対して、p1,p2,p3 で指定した数値を減算します。
^
p1はR、p2はG、p3は Bに対して行なわれます。これはフルカラーモードにおいて、ゆっくりと色を落としていく (フェードアウト) の効果を出す時に有効です。この命令を実行しただけでは実際の画面は更新されません。
「redraw 1」などで画面の更新を行なって初めて反映されます。

%href
gfini


%index
gfinc
画面の色加算
%group
拡張画面制御命令
%prm
p1,p2,p3
p1,p2,p3=0〜255(0) : 色コード（R,G,Bの輝度)

%inst
gfini命令で設定した画面内のすべてのドットに対して、p1,p2,p3 で指定した数値を加算します。
^
p1はR、p2はG、p3は Bに対して行なわれます。これはフルカラーモードにおいて、ゆっくりと色を白にしていく (ホワイトアウト) の効果を出す時に有効です。この命令を実行しただけでは実際の画面は更新されません。
「redraw 1」などで画面の更新を行なって初めて反映されます。

%href
gfini


%index
fxcopy
ファイルのコピー・移動
%group
拡張ファイル操作命令
%prm
p1,"dest",p2
p1=変数    : コピー元のファイル名が格納された文字列型変数名
"dest"     : コピー先のパス名
p2=0,1 (0) : モード指定( 0=コピー / 1=移動 )

%inst
指定したファイルを、別のパスにコピーまたは移動をします。
^
p1で、コピー元のファイル名をあらかじめ格納してある変数名を指定します。
"dest"で、コピー先のディレクトリやドライブを指定します。
p2のモードにより、コピーか移動のどちらかを指定することができます。
p2の指定を省略した場合は、コピーになります。
移動のモードを指定した場合は、元のファイルは削除されます。ただし、移動は同一ドライブ内でなければなりません。
^
fxcopy命令は、Windowsのシェルを使って高速にコピー・移動を行ないます。HSPのbcopy命令よりも高速ですが、 packfile で指定したファイルへのアクセスはできませんので注意してください。
命令の実行に失敗すると、システム変数 statに1が代入されます。正常に終了した場合は、システム変数 statは0になります。

%sample
a="test.bin"
fxcopy a,"c:\\temp"	; "test.bin"をC:\tempにコピー


%index
fxren
ファイル名を変更
%group
拡張ファイル操作命令
%prm
p1,"new name"
p1=変数    : オリジナルのファイル名が格納された文字列型変数名
"new name" : 新規ファイル名

%inst
指定したファイルを、"new name"で指定したファイル名に変更します。
^
p1で、元のファイル名をあらかじめ格納してある変数名を指定します。
"new name"で、新しいファイル名を指定します。
命令の実行に失敗すると、システム変数 statに1が代入されます。正常に終了した場合は、システム変数 statは0になります。


%index
fxinfo
ドライブ情報を取得
%group
拡張ファイル操作命令
%prm
p1,p2,p3
p1=変数   : 情報が格納される変数名
p2=0〜(0) : ドライブ指定
p3=0〜(0) : 情報タイプ指定

%inst
p2で指定したドライブについての情報をp1で指定した変数に代入します。
^
p2で指定するドライブは、0だとカレントドライブ、1ならばAドライブ、2ならばBドライブ…というように1から26までが、A〜Zドライブに対応しています。
p3で、情報タイプを指定することで様々なドライブ情報を取得することができます。p3で指定できる数値は以下の通りです。
^p
  p3 : 変数型   : 情報の内容
 ---------------------------------------------------------
  0  : 数値型   : ドライブの残り容量(byte)
  1  : 数値型   : 1クラスタあたりのセクタ数
  2  : 数値型   : 1セクタあたりのバイト数
  3  : 数値型   : 空きクラスタ数
  4  : 数値型   : トータルのクラスタ数
  8  : 数値型   : ドライブのタイプ
 16  : 文字列型 : ボリュームラベル名
 17  : 文字列型 : ファイルシステム名
 18  : 数値型   : ボリュームシリアル番号
 19  : 数値型   : 使用可能な最大ファイル文字数
 20  : 数値型   : ファイルシステムフラグ
 32  : 文字列型 : ドライブの残り容量(byte) (大容量対応)
^p
指定するタイプにより、p1の変数に代入される型が違います。 タイプ16,17はあらかじめ文字列型の変数を指定しなければなりません。
タイプ8で取得できるドライブのタイプ値の詳細は以下の通りです。
^p
  タイプ : 内容
 --------------------------------
     0   : 不明なドライブ
     1   : ドライブなし
     2   : リムーバブルディスク
     3   : ハードディスク
     4   : リモート(ネットワーク)ドライブ
     5   : CD-ROMドライブ
     6   : RAMディスク
^p
命令の実行に失敗すると、システム変数 statに1が代入されます。正常に終了した場合は、システム変数statは0になります。
^
p3に0を指定して得られるドライブの残り容量は 2Gbyteまでの値となります。
2Gを超える値の場合はp3に32を指定して文字列としてサイズを得る方法をご使用下さい。


%index
fxaget
ファイル属性を取得
%group
拡張ファイル操作命令
%prm
p1,"file"
p1=変数名 : 情報が格納される数値型変数名
"file"    : ファイル名指定

%inst
"file"で指定したファイルの属性を読み出しp1で指定した変数に代入します。
^
読み出した属性は以下のような数値が含まれています。
^p
  属性  : 内容
 --------------------------------
     $1 : 書き込み禁止
     $2 : 隠しファイル
     $4 : システム
    $10 : ディレクトリ
    $20 : アーカイブ
    $80 : 標準タイプ
   $100 : 一時ファイル
   $800 : 圧縮ファイル
  $1000 : オフライン
^p
同時に複数の属性が設定されている場合は、それぞれの値を足した値になります。命令の実行に失敗すると、システム変数 statに1が代入されます。正常に終了した場合は、システム変数 statは0になります。


%index
fxaset
ファイル属性を設定
%group
拡張ファイル操作命令
%prm
"file",p1
"file"    : ファイル名指定
p1=0〜(0) : 設定する属性

%inst
"file"で指定したファイルに、p1で指定した属性を設定します。
^
p1で指定する属性値は以下のような数値が使用できます。
^p
  属性  : 内容
 --------------------------------
     $1 : 書き込み禁止
     $2 : 隠しファイル
     $4 : システム
    $10 : ディレクトリ
    $20 : アーカイブ
    $80 : 標準タイプ
   $100 : 一時ファイル
   $800 : 圧縮ファイル
  $1000 : オフライン
^p
同時に複数のファイル属性を設定する場合は、 それぞれの値を足した値にしてください。
命令の実行に失敗すると、システム変数statに1が代入されます。正常に終了した場合は、システム変数 statは0になります。


%index
fxtget
タイムスタンプを取得
%group
拡張ファイル操作命令
%prm
p1,"file"
p1=変数名 : 情報が格納される数値型の配列変数名
"file"    : ファイル名指定

%inst
"file"で指定したファイルのタイムスタンプ情報を取得して、p1で指定した変数に代入します。
^
ただし、p1の変数は配列変数、数値型で24以上の要素を入れるだけのメモリ確保をしておかなければなりません。
このためfxtgetおよび、fxtset命令で指定する変数はあらかじめ dim命令で、「dim a,24」などの配列宣言をしてから使用してください。取得されたタイムスタンプの情報は配列変数の各要素に代入されます。
たとえば、「fxtget a,"test"」と指定した場合は、変数a.0〜a.23に情報が代入されます。配列の各要素に代入される値の詳細は以下の通りです。
^p
  要素 : 内  容
 ----------------------------------------
   0   : 作成日の年(西暦)
   1   : 作成日の月
   2   : 作成日の曜日(日曜=0)
   3   : 作成日の日
   4   : 作成日の時
   5   : 作成日の分
   6   : 作成日の秒
   7   : 作成日のミリ秒
 ----------------------------------------
   8   : 更新日の年(西暦)
   9   : 更新日の月
  10   : 更新日の曜日(日曜=0)
  11   : 更新日の日
  12   : 更新日の時
  13   : 更新日の分
  14   : 更新日の秒
  15   : 更新日のミリ秒
 ----------------------------------------
  16   : 最終アクセス日の年(西暦)
  17   : 最終アクセス日の月
  18   : 最終アクセス日の曜日(日曜=0)
  19   : 最終アクセス日の日
  20   : 最終アクセス日の時
  21   : 最終アクセス日の分
  22   : 最終アクセス日の秒
  23   : 最終アクセス日のミリ秒
 ----------------------------------------
^p
命令の実行に失敗すると、システム変数statに1が代入されます。
正常に終了した場合は、システム変数statは0になります。


%index
fxtset
タイムスタンプを設定
%group
拡張ファイル操作命令
%prm
p1,"file"
p1=変数名 : 設定する情報が格納されている数値型の配列変数名
"file"    : ファイル名指定

%inst
"file"で指定したファイルのタイムスタンプ情報を、p1で指定した変数のものに変更します。
^
ただし、p1の変数は配列変数、数値型で24以上の要素を入れるだけのメモリ確保をしておかなければなりません。p1で指定する配列変数は、fxtget命令で使用するものと同一形式です。配列変数に指定する値の詳細は、fxtget命令の説明を参照してください。
命令の実行に失敗すると、システム変数 statに1が代入されます。正常に終了した場合は、システム変数 statは0になります。
^
fxtset命令で設定するタイムスタンプは、OSのファイルシステムによっては完全に設定されないことがあります。これは、ファイルシステムが完全な形で時間を保存していないためです。たとえば、Windows95(FAT)の場合は、最終アクセス日の時刻は記録されません。日付のみになります。また秒単位のデータも粗く記録されます。

%href
fxtget


%index
selfolder
フォルダ選択ダイアログ
%group
拡張ファイル操作命令
%prm
p1,"message"
p1=変数名 : 選択されたパス名が格納される文字列型の変数名
"message" : ダイアログに表示される文字列

%inst
Windowsのシステムで使用されている、 フォルダ選択ダイアログを表示してフォルダ名を取得します。
^
選択が正常に終了すると、システム変数 statには0が代入されます。選択時にエラーまたは、キャンセルされた場合には、システム変数 statの値は1になります。
正常に選択された場合には、フォルダまでのフルパス名がp1で指定した変数に代入されます。 また、フォルダ名がシステム変数 refstrに代入されます。
"message" の部分に、ダイアログ表示の時に上に表示される文字列を指定することができます。
指定を「""」にすると、「フォルダを選択してください」という標準的なメッセージが表示されます。


%index
fxshort
DOSファイルネームを取得
%group
拡張ファイル操作命令
%prm
p1,"file"
p1=変数名 : 情報が格納される文字列型変数名
"file"    : ファイル名指定

%inst
"file" で指定したファイルをDOSファイルネームに変換して、p1で指定した変数に代入します。
^
"file" に指定するファイル名は、Windowsで使われているロングファイルネームでなければなりません。
DOS プロンプトのアプリケーションなどに、渡すためのファイル名を取得する場合などに使用できます。
^
尚、 DOSファイルネームをロングファイルネームに変換するには、標準命令のdirlist命令を使うことができます。


%index
fxdir
特殊なディレクトリ名を取得
%group
拡張ファイル操作命令
%prm
p1,p2
p1=変数名 : 情報が格納される文字列型変数名
p2=0〜(0) : 取得するディレクトリ情報の種類

%inst
p2で指定した種類のディレクトリ名を、p1で指定した変数に代入します。
この命令で、 Windowsのシステムで使用される色々なディレクトリ名を取得することができます。
^
p2で指定できる数値は以下の通りです。
^p
   p2 : 取得される内容
 ------------------------------------------------------------
  -2  : Windowsのテンポラリ(一時)フォルダ
  -1  : Windowsのシステム(System)フォルダ
   2  : スタートメニュー「プログラム」グループのフォルダ
   5  :「マイドキュメント」のフォルダ
   6  :「お気に入り」のあるフォルダ
   7  :「スタートアップ」フォルダ
   8  :「最近使った書類」フォルダ
   9  :「送る」(SendTo)のフォルダ
  11  : スタートメニューのフォルダ
  16  : デスクトップのフォルダ
  21  : テンプレート(Template)フォルダ
  26  : ApplicationDataフォルダ
  27  : プリンタデバイスフォルダ
  32  : Internet キャッシュフォルダ
  33  : Internet Cookiesフォルダ
  34  : Internet 履歴フォルダ
^p
指定に誤りがあったり、エラーが発生するとシステム変数 statに1が代入されます。正常に終了するとシステム変数 statは0になります。


%index
fxlink
ショートカットを作成
%group
拡張ファイル操作命令
%prm
p1,"path"
p1=変数名 : ショートカット名が格納されている文字列型変数名
"path"    : 元になるファイルのフルパス名

%inst
p2で指定したファイルへのショートカットを、p1で指定された名前でカレントディレクトリに作成します。
^
p1に指定するのは、ショートカット名が格納された文字列型の変数でなければなりません。また、ショートカット名には拡張子やドライブ、ディレクトリ名を含まない形で指定してください。
(エクスプローラーでは表示されませんが、自動的に拡張子が.lnkになります)
p2には、ショートカット先の場所をフルパスで指定してください。
また、p2に「http://www.onionsoft.net/hsp/」などのURLを指定すると、インターネット・ショートカットが作成されます。
^
指定に誤りがあったり、エラーが発生するとシステム変数 statに1が代入されます。正常に終了するとシステム変数 statは0になります。


%index
lzdist
圧縮解凍先ディレクトリ指定
%group
拡張ファイル操作命令
%prm
"path"
"path" : lzcopy命令の解凍コピー先ディレクトリ

%inst
lzcopy命令の解凍コピー先ディレクトリを指定します。
^
lzcopy命令を実行する場合は、必ずlzdist命令でコピー先をあらかじめ指定しておく必要かあります。


%index
lzcopy
圧縮解凍コピー
%group
拡張ファイル操作命令
%prm
"name"
"name" : 圧縮ファイル名

%inst
Microsoftの compress.exe形式の圧縮ファイルを解凍しながらコピーを行ないます。
^
カレントディレクトリにある"name"で指定された圧縮ファイルを、lzdist命令で指定したディレクトリに解凍された形でコピーします。
"name"には、拡張子を含まない名前(8文字まで)を指定してください。
lzcopy命令を実行する場合は、必ずlzdist命令でコピー先をあらかじめ指定しておく必要かあります。
^
指定に誤りがあったり、エラーが発生するとシステム変数 statに1が代入されます。正常に終了するとシステム変数 statは0になります。





%index
emath
固定小数の精度を指定
%group
拡張入出力制御命令
%prm
p1
p1=2〜30(8) : 固定小数のビット精度

%inst
簡易数学関数命令セット全体で使われる固定小数の精度を設定します。
^
p1で精度のビット数を指定します。たとえば、16を指定すると「整数16bit + 小数16bit」のフォーマットになります。
固定小数のビット精度が高いほど、誤差のない正確な小数点演算が可能になりますが、そのぶん整数部で扱える範囲がせまくなります。
デフォルトでは8bitに設定されています。


%index
emstr
固定小数を文字列に変換
%group
拡張入出力制御命令
%prm
p1,p2,p3
p1=変数名  : 文字列が格納される文字列型変数名
p2=0〜     : 固定小数の値
p3=1〜(10) : 変換される文字列の桁数

%inst
固定小数の値を、小数点を含む文字列に変換します。
^
p1で指定した文字列型変数に、変換後の文字列が代入されます。
p2に、変換の対象になる固定小数の値(が代入されている変数)を指定します。
p3で、変換される桁数を指定することができます。
p3の指定を省略した場合は、10桁になります。


%index
emcnv
文字列を固定小数に変換
%group
拡張入出力制御命令
%prm
p1,"val"
p1=変数名 : 固定小数が格納される数値型変数名
"val"     : 固定小数を示す文字列

%inst
"val" で指定した文字列を固定小数の値に変換して、p1の変数に代入します。
^
たとえば、"3.1415"という文字列を指定すると、それを固定小数に変換して格納します。


%index
emint
固定小数を整数に変換
%group
拡張入出力制御命令
%prm
p1,p2
p1=変数名 : 整数値が格納される数値型変数名
p2=0〜    : 固定小数の値

%inst
"固定小数の値を、通常の32bit整数に変換し、p1で指定した数値型変数に代入します。
^
整数に変換する際には、小数部分はすべて切り捨てられます。


%index
emsin
サインを求める
%group
拡張入出力制御命令
%prm
p1,p2
p1=変数名 : 結果が格納される数値列型変数名
p2=0〜    : 角度(固定小数)

%inst
p2で指定した角度のサイン値を求めて、p1で指定された変数に代入します。
^
p2の角度には、固定小数の値を指定します。
単位は、0から始まって1.0で1回転する数値になっています。
p2の値を32bit整数値として見ると、0から始まり、64で90度、128で180度、192で270度、256で360度となります(固定小数の精度が8bitの場合)。


%index
emcos
コサインを求める
%group
拡張入出力制御命令
%prm
p1,p2
p1=変数名 : 結果が格納される数値列型変数名
p2=0〜    : 角度(固定小数)

%inst
p2で指定した角度のコサイン値を求めて、p1で指定された変数に代入します。
^
p2の角度には、固定小数の値を指定します。
単位は、0から始まって1.0で1回転する数値になっています。
p2の値を32bit整数値として見ると、0から始まり、64で90度、128で180度、192で270度、256で360度となります(固定小数の精度が8bitの場合)。


%index
emsqr
平方根を求める
%group
拡張入出力制御命令
%prm
p1,p2
p1=変数名 : 結果が格納される数値列型変数名
p2=0〜    : 固定小数

%inst
p2で指定した固定小数の平方根(ルート)を求めて、p1で指定された変数に代入します。
^
p2にマイナスの値や0を指定した場合は、0が返されます。


%index
ematan
アークタンジェントを求める
%group
拡張入出力制御命令
%prm
p1,p2,p3
p1=変数名 : 結果が格納される数値列型変数名
p2=0〜    : X値
p3=0〜    : Y値

%inst
Y/Xのアークタンジェントを求めて、p1で指定された変数に代入します。
^
これは、２点間の角度を求める場合に主に使用します。
(X1,Y1) という座標と、(X2,Y2) の座標の角度を求める場合は、座標の差分、X値は(X2-X1)、Y値は(Y2-Y1)を指定します。
p1に代入される角度の値は、emsinやemcos命令で使われる角度の単位(0から始まって、1.0で一回転)と同じです。



%index
regkey
レジストリキー指定
%group
OSシステム制御命令
%prm
p1,"key-name",p2
p1=0〜(0)  : キーグループ指定
"key-name" : キー名指定
p2=0〜1(0) : 既存のキー(0)・新規作成(1) モードスイッチ

%inst
アクセスするレジストリのキーを指定します。レジストリの読み書きをする場合には、最初に必ずregkey命令でキーと読み書きのモードを指定しておかなければなりません。
^
p1では、キーグループを指定します。これは以下の中から選びます。
^p
   p1 : key group
 -----------------------------------------------------
   0  : HKEY_CURRENT_USER
   1  : HKEY_LOCAL_MACHINE
   2  : HKEY_USERS
   3  : HKEY_CLASSES_ROOT
   4  : HKEY_DYN_DATA ( Windows95/98のみ )
   5  : HKEY_PERFORMANCE_DATA ( WindowsNTのみ )
^p
"key-name"では、"Software\\OnionSoftware\\hsed" のように「\\」でツリーを区切って階層指定を行ないます。
最後に、p2で読み書きモードの設定をします。 p2が0の場合は読み出しモードとなり、getreg命令で指定したセクションの内容を読み出すことができるようになります。p2が1の場合は、新規作成モードとなり、 setreg命令で指定したセクションを追加します。
すでに存在するレジストリの読み出し、変更はp2を0に設定してください。
^
regkey命令の実行後は、システム変数statに結果が返ります。
システム変数 statが0以外の場合は、何らかのエラーが起こっていることを示しています。


%index
getreg
レジストリ読み出し
%group
OSシステム制御命令
%prm
p1,"section",p2,p3
p1=変数名  : 読み出すための変数名
"section"  : セクション名指定
p2=0〜1(0) : タイプ指定
p3=1〜(64) : 読み出しサイズ指定

%inst
指定されたセクションの内容をp1で指定した変数に読み出します。
^
あらかじめregkey命令でアクセスするレジストリの位置を指定してから使う必要があります。対象となるレジストリ位置が指定されていない時は、エラーが表示されます。
p2でタイプを指定します、タイプのコードは以下の通りです。
^p
   p2 : type code
 --------------------------
   0  : 数値(32bit)
   1  : 文字列
   2  : バイナリデータ
^p
p3で読み出しサイズを指定します。これは、文字列かバイナリのタイプを選んだ場合に有効になります。
^
命令の実行後は、システム変数 statに結果が返ります。
システム変数 statが0以外の場合は、何らかのエラーが起こっていることを示しています。

%href
regkey


%index
setreg
レジストリ書き込み
%group
OSシステム制御命令
%prm
p1,"section",p2,p3
p1=変数名  : 書き込むための変数名
"section"  : セクション名指定
p2=0〜1(0) : タイプ指定
p3=1〜(64) : 書き込みサイズ指定

%inst
p1で指定された変数の内容を、指定されたセクションに書き込みます。
^
あらかじめregkey命令でアクセスするレジストリの位置を指定してから使う必要があります。対象となるレジストリ位置が指定されていない時は、エラーが表示されます。
p2でタイプを指定します、タイプのコードは以下の通りです。
^p
   p2 : type code
 ---------------------------
   0  : 数値(32bit)
   1  : 文字列
   2  : バイナリデータ
^p
p3で書き込みサイズを指定します。これは、バイナリのタイプを選んだ時にのみ有効になります。
^
レジストリ操作命令では、すべての情報にアクセスが可能ですが、間違ったシステム情報を書き込むと、最悪システムが動作しなくなるなどの深刻な事態を引き起こすので、レジストリの操作は慎重に行なうようにしてください。
^
命令の実行後は、システム変数 statに結果が返ります。
システム変数 statが0以外の場合は、何らかのエラーが起こっていることを示しています。

%href
regkey


%index
regkill
レジストリキー削除
%group
OSシステム制御命令
%prm
p1,"key-name"
p1=0〜(0)  : キーグループ指定
"key-name" : キー名指定

%inst
指定したレジストリのキーを削除します。
^
p1では、キーグループを指定します。これは以下の中から選びます。
^p
   p1 : key group
 ---------------------------------------------------
   0  : HKEY_CURRENT_USER
   1  : HKEY_LOCAL_MACHINE
   2  : HKEY_USERS
   3  : HKEY_CLASSES_ROOT
   4  : HKEY_DYN_DATA ( Windows95/98のみ )
   5  : HKEY_PERFORMANCE_DATA ( WindowsNTのみ )
^p
"key-name" では、"Software\\OnionSoftware\\hsed"のように「\\」でツリーを区切って階層指定を行ないます。
^
命令の実行後は、システム変数statに結果が返ります。
システム変数 statが0以外の場合は、何らかのエラーが起こっていることを示しています。
^
指定したレジストリのキーの下に、さらにキーが含まれている場合には注意が必要です。 OSがWindowsNTの場合は、レジストリキーの下にさらにキーが含まれている場合にはエラーとなり削除できません。
Windows95(98)/NTどちらでも動作するようなスクリプトを作る際には、この点に気をつけてください。


%index
reglist
レジストリ一覧取得
%group
OSシステム制御命令
%prm
p1,p2
p1=変数名  : 読み出すための変数名
p2=0〜1(0) : モード指定

%inst
指定された位置にあるレジストリキーの一覧を取得します。
^
あらかじめregkey命令でアクセスするレジストリの位置を指定しておく必要があります。
p2のモードが、0の時は「セクション一覧」を取得し、モード1の場合は、「キー一覧」を取得することになります。
たとえば、"Software\\OnionSoftware\\hsed"と、"Software\\OnionSoftware\\hspcomm" という2つのレジストリキーが存在したとすると、"Software\\OnionSoftware"の位置から下にあるキー一覧は、「hsed」と「hspcomm」になります。
これにより、レジストリツリーの中にどのようなキーやセクションが存在しているかを調べることができます。

取得された情報は、p1で指定した文字列型の変数に代入されます。
それぞれの項目は改行(\n)で区切られた形で取得されるので、あらかじめ十分な変数のバッファを取っておいた方が安全です。
取得した文字列は、 メモリノートパッド命令やlistbox命令、combox命令でそのまま使用することができます。

%href
regkey



%index
sysexit
システムに終了を通知
%group
OSシステム制御命令
%prm
p1
p1=0〜(0) : 終了タイプ指定

%inst
動作中のWindowsシステムを終了させます。
^
p1で指定されたタイプに従って、 Windowsが終了します。この命令が実行されると、すべてのアプリケーションが終了処理を行ないます。作成中のスクリプトのセーブなど、十分注意して使用してください。
また、sysexit命令の後は、 end命令を入れてHSPのスクリプトも終了させてください。 p1で指定するタイプ指定は以下の通りです。
^p
  タイプ :  内容
 ----------------------------------
    0    : ログオフする
    1    : 再起動をする
    2    : シャットダウンをする
    3    : 電源を切る
^p
電源を切る場合は、「シャットダウン」→「電源を切る」の 2つを連続でリクエストするようにしてください。
^
WindowsNTや2000 などリソース権限があるOSでは、管理者権限でログオンしていないと、この命令を実行することはできません。


%index
pipeexec
パイプ付き実行
%group
拡張入出力制御命令
%prm
p1,"filename",p2
p1=変数    : 標準出力が格納される文字列型変数名
"filename" : 実行されるファイル名
p2=0〜1(0) : ウィンドウ表示スイッチ

%inst
"filename"で指定したファイルを実行します。
実行ファイルは、パイプを通じて標準入力、標準出力の設定取得が可能になります。 pipeexec命令は、おもにwin32コンソールアプリケーション(DOSプロンプトで動作する32bitアプリケーション)の実行を支援するための機能です。
加えて、 16bitアプリケーションおよび、バッチファイルは指定できないので注意してください。

"filename"で実行ファイルを指定します。フルパス指定でない場合は、標準の検索パスが使用されます。
コマンドラインオプションを指定する場合は、ファイル名に続いて DOSプロンプトの要領で指定してください。(例:「notepad.exe readme.txt」)

p1で指定した変数バッファに、実行時の標準出力が格納されます。
あらかじめ「sdim buf,32000」などで大きいバッファを確保したものを指定するようにしてください。
p2でウィンドウの表示ON/OFFを指定します。デフォルトでは、0 (表示しない)
になります。pipeexec命令実行後は、必ず pipegetでプロセス終了までを監視してください。

%href
pipeget
pipeput


%index
pipeget
パイプ付き実行監視
%group
拡張入出力制御命令
%prm
p1
p1=変数 : 結果が格納される文字列型変数名

%inst
pipeexec命令で実行されたプロセスを監視します。
p1に、パイプの情報を取得するバッファとなる文字列型変数を指定する必要が
あります。
pipeexecによるファイル実行の状況は、システム変数statに返されます。
^p
  stat値 | 内     容         | 変数p1の内容
 ---------------------------------------------------------
    0    | 実行終了          | 変化なし
    1    | 実行準備中        | 変化なし
    2    | StdOutputを取得中 | StdOutput
    3    | StdErrorを取得中  | StdError
    4    | 終了処理中        | 変化なし
^p
通常は、pipeexec命令の実行後はpipeget命令でstat値が0になる(実行終了)までwait/await命令を入れながら監視を続けて下さい。
^
１回の実行で取得されるサイズは、p1で指定される文字列型の変数が確保しているバッファサイズまでになりますので、バッファが溢れることはありません。通常4096文字程度のバッファを割り当てるようにしていれば問題はないでしょう ( 例:「sdim buf,4096」)。

%href
pipeput

%sample
	sdim ln,4096
	sdim buf,32000

	pipeexec buf,"notepad.exe",1
	if stat : dialog "実行できませんでした" : end

	mes "実行中..."

	repeat
	pipeget ln          ; パイプの取得/更新
	if stat=0 : break   ; 実行終了ならループを抜ける
	wait 10
	loop

	mes "実行完了..."
	mes buf

	stop


%index
pipeput
パイプに1byte送る
%group
拡張入出力制御命令
%prm
p1
p1=0〜255(0) : 送信コード

%inst
pipeexec命令で実行中のプロセスに対して、p1で指定したコードを標準入力として送ります。

%href
pipeget



%index
dirlist2
拡張ディレクトリ取得
%group
拡張ファイル操作命令
%prm
p1,"filemask",p2,p3
p1=変数名  : ディレクトリ情報全体のサイズ
"filemask" : 一覧のためのファイルマスク
p2=0〜(0)  : 取得モード
p3=0〜(0)  : ディレクトリ区切り記号
%inst
ディレクトリの情報を取得します。
標準命令のdirlistよりもさらに詳しく検索することができます。
dirlist2命令は、3つのステップで処理が完了します。
^p
	1.dirlist2h命令で取得するものを選択
			↓
	2.dirlist2命令で取得を開始
			↓
	3.dirlist2r命令で結果を取得
^p
dirlist2命令は、決められたモードでディレクトリ情報の取得を行ないます。
結果は、dirlist2r命令で取得することができます。
dirlist2命令が正常に終了した場合には、システム変数statに情報を取得した
ファイルの合計数が代入されます。取得モードの詳細は以下の通りです。
^
"filemask"は、dirlist命令と同様に「*.*」のようなワイルドカード指定による検索ファイル名となります。
^
p3で、取得時のディレクトリ区切り記号を指定することが可能です。0か、省略をした場合は「\」が使用されます。
^
モードの値は、合計して組み合わせることが可能です。
モード1 (bit0) を指定した場合には、 カレントディレクトリ以下全てのディレクトリを再帰検索してファイル情報を取得します。
^p
  モード :  取得される内容
 ---------------------------------------------------------------
    0    :  カレントディレクトリのファイル一覧
    1    :  カレントディレクトリ以下のすべてのファイル一覧
    2    :  隠しファイル、システム属性ファイルを一覧に含める
^p
%href
dirlist2h
dirlist2r


%index
dirlist2h
拡張ディレクトリ取得設定
%group
拡張ファイル操作命令
%prm
p1,p2
p1=0〜(0) : 一覧リストフラグ
p2=0〜(0) : リスト区切り記号
%inst
dirlist2命令によるディレクトリの情報取得の詳細を設定します。
実際の情報取得の方法については、dirlist2命令の説明を参照してください。
^
p1の一覧リストフラグにより指定された項目が、区切り記号により分けられテキスト情報として取得できるようになります。

一覧リストフラグは、以下の通りです。
^p
  フラグ : 取得される内容
 -----------------------------------------------------------------
     1   : カレントからの相対パスによるファイル名
     2   : パス名を除いたファイル名
     4   : ファイルのサイズ
     8   : ファイルの更新日付
    16   : ファイルの更新時間
   128   : カレントからの相対パス名のみ
^p
フラグ値は、加算して組み合わせることができます。p1が0か省略された場合は、すべてのフラグを指定したことになります。
p2でリストを区切る記号を指定することができます。p2が0か省略された場合は、「,」が指定されます。
%href
dirlist2
dirlist2r


%index
dirlist2r
拡張ディレクトリ取得2
%group
拡張ファイル操作命令
%prm
p1
p1=変数名 : ディレクトリ情報を取得する変数
%inst
dirlist2命令により取得された情報を変数に代入します。
必ずdirlist2命令で返されたサイズ以上のメモリが確保されている変数に対して実行する必要があります。
実際の情報取得の方法については、dirlist2命令の説明を参照してください。
%href
dirlist2
dirlist2h


