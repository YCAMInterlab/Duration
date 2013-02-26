# [Duration](http://duration.cc)

## クリエイティブコーディングのためのタイムライン

Durationはタイムラインに沿って、値の変化をコントロールします。シンプルな1画面のインターフェースを備えたクロスプラットフォームのスタンドアローンアプリケーションで、複数のトラックを使ってタイムライン編集をする事ができます。

このアプリケーションによるタイムラインからの値はOSCによって送信されます。また、外部からのOSCメッセージで、アプリケーションを操作する事も可能です。

![Durationのインターフェース](http://www.duration.cc/assets/images/duration_screen.png)

Durationはオープンソースで公開されています。ご自分のアート作品制作や、リサーチ活動、またコマーシャルなプロジェクトにおいても完全にフリーで使うことが出来ます。このプロジェクトは[openFrameworks](http://www.openframeworks.cc)を使用して作られており、そのシェアに関しての考え方を引き継いでいます。

Duration is a project by [James George](http://www.jamesgeorge.org), co-developed by [YCAMInterLab](http://interlab.ycam.jp/en/) during the [Guest Research Project v.2](http://interlab.ycam.jp/en/projects/guestresearch/vol2)

## ダウンロード

Durationをダウンロードする：

[Mac OS X 10.6+](http://www.duration.cc/downloads/Duration_003_OSX.zip)

[Windows XP and 7](http://www.duration.cc/downloads/Duration_003_Windows.zip)

[Linux 64bit](http://www.duration.cc/downloads/Duration_003_Linux64.zip)  
Thanks to [Philip Whitfield](http://underdoeg.com/) for the Linux build.

[全プラットフォーム用のソースコード](http://www.duration.cc/downloads/DurationSource_003.zip)

Windowsユーザーの方は[MSVC++ Runtime](http://www.microsoft.com/en-us/download/details.aspx?id=5555)と[OpenAL](http://connect.creativelabs.com/developer/Wiki/OpenAL%20Installer%20for%20Windows.aspx)のインストールが必要になる場合があります。

## デモビデオ

Readmeを読む時間がありませんか？最新の紹介映像がここにあります。  
https://vimeo.com/59654979

## ライセンス

    Copyright 2012-2013 James George and YCAMInterLab
    
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.


## デフォルトの言語を変更する

Durationは現在、英語、日本語、フランス語、イタリア語で使用する事ができます。

言語を変更するには、Duration/data/settings.mxlを開き、languageタグを変更してDurationを再起動します。

Joanie LeMercier, Takayuki Ito, Francesco Fantoni、翻訳のご協力ありがとうございました。もし、他の言語の翻訳を提供してもよいと思われる方がいらっしゃいましたら、languagefile.csvを開き、翻訳を追加していただいたあと、私にpullリクエストを使って、もしくはemailで送ってください。次のリリース時に追加します！

## Durationのインターフェースを使用する

### プロジェクトを管理する

Durationのプロジェクトファイルはトラックのデータと、インターフェースの設定だけで構成されています。それらのデータは.xmlファイルと、トラック名と設定を含む.durationprojファイルの形式で保存されます。

Durationは、~/Documents/Durationフォルダにあるプロジェクトファイルを見るように設定されています。ここに保存されたプロジェクトはアプリケーション内のドロップダウンメニュー内で表示されます。このフォルダ内でなくともプロジェクトファイルを保存しておく事は可能ですが、その場合はどこに保存したのか覚えておく必要があるでしょう。

### トラックを管理する

トラックはDurationの全てだと言えるでしょう。トラックを自分のプロジェクトに追加するには、「トラックを追加する」もしくは「Add Track」と書かれたドロップダウンからトラックのタイプを選択します。 すると指定したタイプのトラックが追加されます。トラックのヘッダ部分に書かれているトラック名をクリックして、トラックに名前をつけてください。この名前はDurationから送信される全てのOSCメッセージに使用されます。つまり、トラック名は同時にOSCのアドレスとしての意味を持ちます。複数のトラックで同じ名前を使用する事も出来ますが、受信をする側のアプリケーションの処理を難しくしてしまうかもしれません。

トラックの名称の部分にメッセージのルーティング情報を入れるのも一般的です。例えば/mytrack/position/xなどは良いトラック名だと言えるでしょう。もし、トラック名の先頭にスラッシュを付け忘れても、Durationは送信時に付加して送信します。トラックは、トラック名のずっと右側にある削除ボタンをクリックし、確認をすることで削除する事ができます。

### トラックのタイプ

Durationは標準で6つのタイプのトラックを使用する事が可能です。それぞれのトラックでは、キーフレームを使って、任意の特定の時間での値や振る舞いを指定します。キーフレーム間の値がどのようにスムーズに補完されるか、をキーフレームごとに指定する事が可能です。

#### バン/Bangs

バンは最もシンプルなトラックで、再生ヘッドがそれぞれのキーフレームを通過するときに、一度だけパラメータの無いbangメッセージを送信します。

#### フラグ/Frags

フラグはbangsに似ていますが、各メッセージにテキストを付けて送信する事が可能です。

#### スイッチ/Switch
 
スイッチはトラック中のエリアごとにON/OFFの指定をする事が出来ます。再生ヘッドがON/OFFが切り替わる部分を通過する際に、メッセージが送信されます。

#### カーブ/Curve

カーブを使うと、指定した最小値と最大値の間で値をスムーズに変化させる事が出来ます。キーフレームを右クリックするとメニューが表示され、キーフレーム間の補完方法を指定する事が出来ます。

#### LFOs

アニメーションの制作等用に、ローフリークエンシーオシレーターによるサイン波とノイズを使用する事が出来ます。

#### カラー/Color

カラートラックでは、色のサンプルパレットとして画像を使用し、時間の経過に従ってスムーズに色を変化させる事が出来ます。各キーフレームでは画像パレット中の、色をサンプルする位置を指定します。キーフレーム間では、画像パレット内で色のサンプル位置が移動していくことで、滑らかに色が変化します。

#### オーディオ/Audio

オーディオトラックを使用すると、トラック内への音の波形表示、サウンドの再生、またFFTの結果をOSC経由で送信する事が出来ます。現状、一つのプロジェクト内では一つのオーディオトラックのみが使用可能です。また、プロジェクトの全長はオーディオトラックの長さと同じである必要があります。

### プロジェクトの長さの設定

全てのプロジェクトはある決まった長さを持ちます。その長さを変更するには、画面上部、再生ヘッドの現在時刻が表示されている部分の下に表示されている時間（これがプロジェクトの長さです）をクリックし、新しい長さを入力します。入力する長さの値は、必ずタイムコードのフォーマット、HH:MM:SS:MILS（HH:時間、MM:分、SS:秒、MILS:ミリ秒）で書かれていなければなりません。プロジェクトの長さを短くすると、新しい長さに収まらないキーフレームがカットされてしまう事があります。


### イン点とアウト点

イン点とアウト点を設定すると、全体のシーケンスの中の、指定した場所のみを再生する事が可能です。iとoのホットキーを使用してイン点とアウト点を設定し、再生範囲を指定しましょう。もしくは時間の目盛りのすぐ下の部分をドラッグする事でもイン点とアウト点の指定が可能です。Altキー+i、Altキー+oで現在のinとoutを削除する事も出来ます。

### BPMの設定

Durationでは、BPMの値に連動した基本的な4/4拍子のガイドを表示する事が出来ます。これを使う事でキーフレームを1/4毎のガイドに正確にスナップさせる事が出来ます。BPMを変更するには、表示されているBPMの数値をドラッグします。　

ノート：より複雑なガイドの表示は、将来的なアップデートの中で計画されています。

### OSCの設定

Durationウィンドウの右上の部分で、OSCの送受信に関しての設定を行う事が出来ます。受信に使用するネットワークポートと送信先のネットワークアドレス、送信に使用するネットワークポートを指定します。それぞれのトラックのヘッダー部分にあるトグルスイッチを用いる事で、トラックごとに独立してOSCの送信受信を行うかどうかを指定する事ができます。

### ショートカット

* ノート: WindowsとLinuxはCTRLキーを使用しますが、MacではCOMMANDキーを使用します。

<table>
    <tr>
        <td style="font-weight:bold">機能</td><td style="font-weight:bold">ショートカット</td>
    </tr>
    <tr>
        <td>再生／停止</td><td>スペース</td>
    </tr>
    <tr>
        <td>保存</td><td>command+s</td>
    </tr>
    <tr>
        <td>選択項目をカット</td><td>command+x</td>
    </tr>
    <tr>
        <td>選択項目をコピー</td><td>command+c</td>
    </tr>
    <tr>
        <td>選択項目を貼付け</td><td>command+v</td>
    </tr>
    <tr>
        <td>一つ戻る</td><td>command+z</td>
    </tr>
    <tr>
        <td>やり直し</td><td>shift+command+z</td>
    </tr>
    <tr>
        <td>イン点を設定</td><td>i</td>
    </tr>
    <tr>
        <td>アウト点を設定</td><td>o</td>
    </tr>
    <tr>
        <td>イン点をクリア</td><td>alt+i</td>
    </tr>
    <tr>
        <td>アウト点をクリア</td><td>alt+o</td>
    </tr>
    <tr>
        <td>選択したトラックの全キーフレームを選択</td><td>command+a</td>
    </tr>
    <tr>
        <td>選択したトラックの全キーフレームを選択項目に追加</td><td>command+shift+a</td>
    </tr>
    <tr>
        <td>選択された全てのキーフレームを削除</td><td>deleteもしくはbackspace</td>
    </tr>    
    <tr>
        <td>キーフレームを少しずつ動かす</td><td>矢印キー</td>
    </tr>    
    <tr>
        <td>キーフレームをもう少しずつ動かす</td><td>shift+矢印キー</td>
    </tr>    

</table>

## Durationに接続する

Durationは単体で使用してもあまり役に立ちません。ですので是非、OSCを使用して他のリアルタイムの環境と接続してみましょう。

### 出力されたものを受信する

Durationは、その全てのデータをバンドルされたOSCで送信します。値が変化したときには常に更新された値を送信します（単に編集中に値が変化したときでも、再生されている時も、常にです）。OSCメッセージは次のようなフォーマットになっています。

     /track/display/name <アーギュメント（値）>

トラックはそれぞれタイプごとに異なるアーギュメントを必要とします。
<table>
    <tr>
        <td style="font-weight: bold">トラックのタイプ</td>
        <td style="font-weight: bold">アーギュメント</td>
    </tr>
    <tr>
        <td>バン/Bangs</td>
        <td>アーギュメントはありません</td>
    </tr>
    <tr>
        <td>フラグ/Frags</td>
        <td>フラグごとのテキストとして1つの文字列（空白でも可）</td>
        </tr>
    <tr>
        <td>スイッチ/Switch</td>
        <td>1つのInt32の値。1/0の値をとり、それぞれON/OFFを表します。</td>
    </tr>
    <tr>
        <td>カーブ/Curve and LFO</td>
        <td>再生ヘッド上のカーブの値として、1つのFloatの値。値は設定した最大値と最小値の間になります。</td>
    </tr>
    <tr>
        <td>カラー/Color</td>
        <td>3つのInt32の値。それぞれ赤、緑、青を表し、0から255の間の値になります。</td>
    </tr>
</table>


### トラック情報に関するメッセージ

Durationは、新しいプロジェクトが読み込まれたり、もしくは再生が始まる時に/duration/info というアドレスで、現在のプロジェクトの全てのトラックに関する情報を含んだメッセージを送信します。メッセージのトラック情報は、一つのカーブトラックにつき4つのアーギュメント、他のタイプのトラック1つにつき2つのアーギュメントで構成されます。最初のアーギュメントは常にトラックタイプを表す文字列です。二つ目はトラックのヘッダーに表示されているトラック名になります（これは常にそのトラックから送信されるOSCのアドレスと一致します）。カーブトラックには、そこにさらに2つのアーギュメントが続いて、それぞれ、そのカーブトラックで指定された最小値と最大値を表すfloatの値です。

2つのカラートラックと一つのカーブトラック、さらに一つのフラグトラックを含むプロジェクトでは、このメッセージは次の様になります。
     
    /duration/info Colors /colortrack_one Colors /colortrack_two Curves /accelerate -1.5 2.0 Flags /flagtrack

### DurationをOSC経由で操作する

Durationは、常にOSCインプットからのメッセージを使って操作出来る状態にあります。

<table>
    <tr>
        <td style="font-weight: bold">コマンド</td>
        <td style="font-weight: bold">OSCアドレス</td>
        <td style="font-weight: bold">アーギュメント</td>
        <td style="font-weight: bold">説明</td>
    </tr>
    <tr>
        <td>新規プロジェクト</td>
        <td>/duration/new</td>
        <td>プロジェクトのパス(String)</td>
        <td>"プロジェクトのパス"で指定した場所に新しいプロジェクトを作成して開きます。それまでに作成していたプロジェクトは保存されません。スラッシュが先頭についている場合、絶対パスでの指定になります。そうでない場合は~/Documents/Durationからの相対パスでの指定になります。</td>
    </tr>
    
    <tr>
        <td>プロジェクトを開く</td>
        <td>/duration/open</td>
        <td>プロジェクトのパス(String)</td>
        <td>"プロジェクトのパス"で指定したプロジェクトを開きます。それまでに作成していたプロジェクトは保存されません。スラッシュが先頭についている場合、絶対パスでの指定になります。そうでない場合は~/Documents/Durationからの相対パスでの指定になります。</td>
    </tr>
    <tr>
        <td>プロジェクトを保存する</td>
        <td>/duration/save</td>
        <td>-</td>
        <td>現在のプロジェクトに変更された全てを保存します。</td>
    </tr>

    <tr>
        <td>再生を開始する</td>
        <td>/duration/play</td>
        <td>-</td>
        <td>プロジェクトが既に再生している場合は特になにもしません。再生が開始されると/duration/infoメッセージが送信されます。</td>
    </tr>
    <tr>
        <td>再生を停止する</td>
        <td>/duration/stop</td>
        <td>-</td>
        <td>既に再生が止まっている場合は、再生ヘッドがシーケンスの先頭位置に移動します。</td>
    </tr>
    <tr>
        <td>プロジェクトの長さを設定する</td>
        <td>/duration/setduration</td>
        <td>秒(Float)</td>
        <td>現在のプロジェクトの長さを、指定した秒数にします。もし指定した長さが現在の長さよりも短い場合、それ以降にあった値が消えてしまう事があります。</td>
    </tr>
    <tr>
        <td>プロジェクトの長さを設定する</td>
        <td>/duration/setduration</td>
        <td>ミリ秒(Int)</td>
        <td>現在のプロジェクトの長さを、指定したミリ秒の長さにします。もし指定した長さが現在の長さよりも短い場合、それ以降にあった値が消えてしまう事があります。</td>
    </tr>
    <tr>
        <td>プロジェクトの長さを設定する</td>
        <td>/duration/setduration</td>
        <td>タイムコード(String)</td>
        <td>現在のプロジェクトの長さを、HH:MM:SS:MILSのフォーマットで指定したタイムコードの長さにします。もし指定した長さが現在の長さよりも短い場合、それ以降にあった値が消えてしまう事があります。</td>
    </tr>
    <tr>
        <td>移動</td>
        <td>/duration/seektoposition</td>
        <td>ポジション(Float)</td>
        <td>再生ヘッドの位置を0から1.0の間の値でセットします。</td>
    </tr>
    <tr>
        <td>移動</td>
        <td>/duration/seektosecond</td>
        <td>秒(Float)</td>
        <td>再生ヘッドの位置を、指定した秒の位置に移動します。</td>
    </tr>
    <tr>
        <td>移動</td>
        <td>/duration/seektomillis</td>
        <td>ミリ秒(Int)</td>
        <td>再生ヘッドの位置を、指定したミリ秒の位置に移動します。</td>
    </tr>
    <tr>
        <td>移動</td>
        <td>/duration/seektotimecode</td>
        <td>タイムコード(String)</td>
        <td>再生ヘッドの位置を、HH:MM:SS:MILSで指定したタイムコードの位置に移動します。</td>
    </tr>
    <tr>
        <td>OSCの受信を有効/無効化する</td>
        <td>/duration/enableoscin</td>
        <td>有効/無効(Int)</td>
        <td>1でOSC入力を有効化、0で無効化します。無効化する場合、これがDurationに送る最後のメッセージになりますので、ご注意ください。</td>
    </tr>
    <tr>
        <td>OSCの入力を有効/無効化する</td>
        <td>/duration/enableoscin</td>
        <td>トラック名(String); 有効/無効(Int)</td>
        <td>トラック名で指定したトラックのOSC入力を、1で有効化、0で無効化します。</td>
    </tr>
    <tr>
        <td>OSCの出力を有効/無効化する</td>
        <td>/duration/enableoscout</td>
        <td>有効/無効(Int)</td>
        <td>1でOSC出力を有効化、0で無効化します。</td>
    </tr>
    <tr>
        <td>OSCの出力を有効/無効化する</td>
        <td>/duration/enableoscout</td>
        <td>トラック名(String); 有効/無効(Int)</td>
        <td>トラック名で指定したトラックのOSC出力を、1で有効化、0で無効化します。</td>
    </tr>
    <tr>
        <td>OSC送信レートの指定</td>
        <td>/duration/oscrate</td>
        <td>レート(Float)</td>
        <td>1秒間に送信されるOSCメッセージの数をレートで指定します。標準では30に設定されています。</td>
    </tr>
    <tr>
        <td>トラックを追加する</td>
        <td>/duration/addtrack</td>
        <td>トラックタイプ(String); トラック名(String,Optional); XMLファイル(String,Optional)</td>
        <td>新しく指定したタイプのトラックを作成し、追加します。トラックタイプはbangs, flags, switches, curves, colorsもしくはaudioです。必要であれば、トラック名を指定する事も、外部XMLファイルのパスを指定して読み込む事も出来ます。</td>
    </tr>
    <tr>
        <td>最小最大値の指定</td>
        <td>/duration/valuerange</td>
        <td>トラック名(String); 最小値(Float); 最大値(Float)</td>
        <td>トラック名で指定したカーブトラックの最小値と最大値を変更します。</td>
    </tr>
    <tr>
        <td>最小値の指定</td>
        <td>/duration/valuerange/min</td>
        <td>トラック名(String); 最小値(Float)</td>
        <td>トラック名で指定したカーブトラックの最小値を変更します。</td>
    </tr>
    <tr>
        <td>最大値の指定</td>
        <td>/duration/valuerange/max</td>
        <td>トラック名(String); 最大値(Float)</td>
        <td>トラック名で指定したカーブトラックの最大値を変更します</td>
    </tr>
    <tr>
        <td>カラーパレットの指定</td>
        <td>/duration/colorpalette</td>
        <td>トラック名(String); カラーパレットのパス(String)</td>
        <td>カラーパレットのパスで指定した画像をカラートラックのパレットとしてLoads the image at PalettePath for target Colors track</td>
    </tr>
    <tr>
        <td>オーディオクリップの指定</td>
        <td>/duration/audioclip</td>
        <td>クリップのパス(String)</td>
        <td>オーディオトラックにサウンドクリップを読み込みます。指定をする際には、プロジェクトにオーディオトラックが存在している必要があります。</td>
    </tr>

</table>

### リモートコントロール

Durationをダウンロードすると"DurationRemote"と呼ばれるシンプルなアプリケーションが付いてきます。これを使ってコントロールメッセージをテストする事が出来ます。

![DurationRemote](http://www.duration.cc/duration_remote.png)

### OSCからのデータをトラックに記録する

Durationには外部からのOSCデータをカーブトラックやバントラックにキーフレームとして記録するための、実験段階の機能があります。
受信しているシグナルをカーブトラックに記録するには、トラック名を、受信しているOSCのアドレスと同じになるように設定します。また、受信するOSCの最初のパラメーターは必ずfloatである必要があります。

もし、受信しているメッセージのアドレスがカーブトラックの名称と一致していて、かつ値が適切であった場合、そのトラックの背景が赤く点滅しはじめます。これはレコーディングの準備が整っている事を示しています。再生ボタンをクリックすると、即座にトラックに入力されているデータが記録されていきます。バントラックを使用して断続的なメッセージを、カーブトラックを使用してデータストリームをレコーディングしてみましょう。レコーディングを止めてそのトラックを単純に再生したい時は、そのトラックのOSC入力を無効にしてください。

* ノート: バンドルされているRecordingDataGeneratorを使って、この機能をテストする事が出来ます。Durationのトラック名をRecordingDataGeneratorと同じにしてテストしてみてください。

![RecordingGenerator](http://www.duration.cc/duration_datagenerator.png)

* ノート: この機能はまだ実験段階の機能で、おかしな動作をする事があります。トラックで指定した範囲に収まらない値を受信したり、既存のデータの上に上書きしたりしないようにしてください。大量の管理出来ないキーフレームが登録されたりします。こういった例外的なケースに関し、現状のDurationはまだ全てに上手に対応しきれていません。

## Durationをハッキングする

Durationはオープンソースかつ無償で提供されます。タイムラインは非常に多くの状況で使用されるため、一つのアプリケーションでその全ての目的を満たす方法はありません。そういった状況を考え、Durationは解体出来るように作られています。いくつかの方法でDurationをカスタマイズする事が出来ます。

### ソースコードをダウンロードする

Durationのリリースは、バイナリーアプリケーションともに、ソースコードのパッケージも、このレポジトリのダウンロードセクションに入っています。ソースコードのパッケージはアプリケーションと、全ての依存ファイル、そしてサポートしている全プラットフォームのプロジェクトファイルを含んでいます。これがDurationをハックするための、最も簡単な方法です。しかし、もしもプロジェクト自体の開発自体に貢献してくださるのであれば、このプロジェクトや、依存しているプロジェクトをGithub上でフォークし、更新した部分に関してのPullリクエストをしてください。

### Githubからコンパイルする

Githubのデータからプロジェクトをコンパイルするには下記の三つの手順が必要です。
 - openFrameworksを、githubのdevelopブランチからフォークする
 - Durationのプロジェクト自体も、openFrameworks/apps/フォルダにフォークします。
 - 全ての依存しているadd-onをopenFrameworks/addons/にフォークします。

Durationのレポジトリにはclone_addons.shファイルが入っています。このファイルは、次の様にターミナルから実行する事が出来ます。

    $ ./openFrameworks/apps/Duration/clone_addons.sh 

これで、必要な全ての依存ファイルがopenFrameworks/addons/にダウンロードされ、プロジェクトのコンパイルが出来る様になるはずです。

## Version History
 - Alpha 004 released February 26th 2013
 - Alpha 003 released February 2nd 2013
 - Alpha 002 released September 23rd 2012
 - Alpha 001 released August 15th 2012

## Supported By ##

Duration is a project by [James George](http://www.jamesgeorge.org), co-developed by [YCAMInterLab](http://interlab.ycam.jp/en/) during the [Guest Research Project v.2](http://interlab.ycam.jp/en/projects/guestresearch/vol2)


<p align="center">
    <img src="http://www.jamesgeorge.org/images/ofxtimeline/github/logo_ycam.png" />
</p>

## Additional Support

![Additional Support](http://www.jamesgeorge.org/images/ofxtimeline/github/SupportedBy.png)

[Playmodes](http://www.playmodes.com/) who graciously funded and shaped the project in it's early stages for [BlueBeams](https://vimeo.com/35931265) .

[The Frank-Ratchye STUDIO for Creative Inquiry](http://studioforcreativeinquiry.org/) who have supported the [RGBDToolkit](http://www.rgbdtoolkit.com) project for which ofxTimeline was first created.

[Eyebeam](http://http://eyebeam.org/) who supported further development of ofxTimeline during my residency in Spring 2012.


Built with openFrameworks and ofxTimeline
