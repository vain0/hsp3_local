;
;	HSP help manager用 HELPソースファイル
;	(先頭が「;」の行はコメントとして処理されます)
;

%type
内蔵命令
%ver
3.4
%note
ver3.4標準命令
%date
2009/08/01
%author
onitama
%url
http://hsp.tv/
%port
Win
Let



%index
mci
MCIにコマンドを送る
%group
マルチメディア制御命令
%prm
"strings"
"strings" : MCIコマンド文字列

%inst
MCI（Multimedia Control Interface）にコマンド文字列を送ります。MCIに対応したMIDI再生、ムービー再生などを行なうことができます。
^
MCIコマンドの詳細については、Windows MCIの解説書をお読みください。ここでは簡単な使用法だけを紹介しておきます。
^p
	mci "play filename"
^p
で、"filename"のファイルを再生します。たとえば、"play aaaa.mid"ならば、"aaaa.mid" というMIDIファイルを演奏します。拡張子がaviなら動画の再生、wavならPCMの再生、その他MCIに登録されているデバイスを再生することができます。
^p
	mci "open filename alias abc"
^p
で、"filename"のファイルをオープンして、これ以降 "abc"という名前をエイリアスにすることができます。この後で、
^p
	mci "play abc"    ; デバイスの再生
	mci "stop abc"    ; デバイスの再生ストップ
	mci "close abc"   ; デバイスのクローズ
^p
などの指定をすることができるようになります。
mciに命令を送った場合の結果はシステム変数 statに反映されます。
stat が-1の場合は、mci命令を解釈する時点でエラーが出ていることを示しています。また、mciでステータスを読み出すコマンドを送った場合も、stat に結果が反映されます。
%portinfo
HSPLet時は、以下の命令のみサポートされます。

open file alias name 
play name 
seek name to position 
status name position 
stop name 
close name 

WAV/AIFF/AU/MIDI/MP3 をサポートしています。 MP3の再生には付属の jl1.0.jar が必要です。


%index
mmplay
サウンド再生
%group
マルチメディア制御命令
%prm
p1
p1=0〜(0) : 再生するバッファの番号

%inst
mmload命令によって読み込まれたメディアを再生します。
p1でバッファの番号を指定することで、mmload命令によって読み込まれた複数のメディアから、どれを再生するかを選びます。
^
mmplay命令は、 通常サウンドの再生がスタートするとともにHSPは次の命令へと進みます。
ただし、mmload命令で読み込みモード２を指定している場合は、サウンドの再生が終了するまで次の命令へは進まなくなります。
^
すでにサウンドが再生されている状態で、さらにmmplay命令を実行すると、前のサウンド再生を終了してから、新しく指定したサウンドの再生を始めます。
ただし、MIDIの再生中にPCMを再生するなど異なるデバイスの場合は、 再生が中止されることはありません。
^
AVI(動画)の再生は、 mmplay命令が実行された時点で操作先となっているウインドゥのカレントポジションを左上として表示されます。
ただし、 mmload命令のモードに+16(ウィンドウ全体で再生)が指定されている場合は、ウィンドウの表示面いっぱいに再生されます。
この場合、画面サイズや縦横の比率には関係なくウィンドウのサイズに変倍されるので、あらかじめ動画の画面サイズや縦横比を考えた上で利用して下さい。

%href
mmload
mmstop




%index
mmload
サウンドファイル読み込み
%group
マルチメディア制御命令
%prm
"filename",p1,p2
p1=0〜(0)  : 割り当てるバッファの番号
p2=0〜2(0) : 割り当てるモード

%inst
メディアデータのファイルを登録します。
読み込むことができる形式は以下の通りです。
^p
WAV形式  : 拡張子 WAV         : Windows標準のPCM音声データ。
AVI形式  : 拡張子 AVI         : Windows標準の動画データ
MID形式  : 拡張子 MID         : 標準MIDIファイルデータ。
MP3形式  : 拡張子 MP3         : MP3形式音声データ
ASF形式  : 拡張子 ASF,WMV,WMA : Windows Media形式音声データ
MPEG形式 : 拡張子 MPG         : MPEG形式動画データ
オーディオCD : ファイル名 "CD : トラック番号" :
               (CDの音声トラック部分が対象になります)
^p
たとえば、オーディオCDのトラック3を指定する場合は、
^p
	mmload "CD:3",1
^p
のようにします。
p1は、割り当てるバッファの番号となります。これは、再生する時に必要となる0以上の整数値です。
複数のメディアファイルを取り扱うには、別々のバッファ番号に割り当てをしておく必要があります。
mmload命令は、指定されたファイルが2MB以下の音声(WAV)ファイルの場合は、メモリに内容を読み込んでおきリアルタイムに再生可能な状態にします。それ以外の場合は、ファイル名だけが登録され、実際のファイルの読み出しはメディア再生時(mmplay実行時)に行なわれることになります。
^
p2はモードを設定します。以下の値を指定することができます。
^p
モード0   : 指定したファイルは通常の再生を行う
モード1   : 指定したファイルは無限ループで再生を行う
モード2   : 指定したファイルは再生終了まで待つ
モード3   : (CDのみ)指定されたトラック以降を再生する
モード+16 : (AVIのみ)対象ウィンドウ全体で再生
^p
モード２を指定すると、指定したファイルが再生された場合、 HSPはその再生が終了するまで次の命令を実行しなくなります。
mmload命令はデータの情報をメモリ上にストックしておくだけで、すぐに演奏が始まるわけではありません。
^
モード+16は、AVI(動画)ファイルでのみ使用できるモードです。
モード0〜2に16を足した値を指定することで、表示対象になっているウインドゥのサイズいっぱいに動画が再生されます。
動画ファイルの判断は拡張子によって行なっています。
^
MP3形式、およびASF形式はOSがサポートしている場合にのみ再生が可能です。
(初期のWindows95、Windows98ではWindows Media Player  5.2以降がインストールされている必要があります。)
^
実際の再生は、mmplay命令によって行ないます。

%href
mmplay
mmstop
%portinfo
HSPLet時、WAV/AIFF/AU/MIDI/MP3 をサポートしています。 MP3の再生には付属の jl1.0.jar が必要です。



%index
mmstop
サウンド停止
%group
マルチメディア制御命令
%prm


%inst
mmplay命令によるメディア再生を停止します。

%href
mmplay
mmload


