# tcount
關鍵詞統計 chrome extension

![](https://i.imgur.com/uwSHHhu.png)

## 簡介
關鍵詞統計為一 google extension，當點擊該 extenstion 時，會出現一個頁面內出現的關鍵詞出現次數的統計圖。

## 安裝
1. git clone https://www.github.com/hrh47/tcount.git
2. cd tcount/
3. 參考[這裡](https://www.alexclassroom.com/internet/google/google-chrome/how-to-manually-install-chrome-extension/#_Chrome__APK_Downloader)，安裝 chrome extension。
選取擴充功能目錄選擇「tcount/plugin」
4. npm start

## 使用到的 library
* Chart.js: 一款彈性很高的圖表 javaScript library，支援八種常見的統計圖表類型。
* node-readability: 將網頁的主要部份擷取出來，去除廣告等會影響關鍵詞結果的部份。
* html-to-text: 將 node-readability 擷取後的的網頁去除 html tags，將文字取出。

## 前端 chrome extension
* 將點擊 extension 時的網址傳回後端 nodejs server。
* 後端 server 傳回結果時，若網頁沒有任何關鍵詞，則顯示"沒有找到關鍵詞"；若有關鍵詞，則使用 chart.js 顯示出現次數前十高的關鍵詞次數的長條圖。

## 後端 server
* 將前端傳來的網址經由 node-readability 下載網頁內容。
* 將網頁內容以 html-to-text 取得文字。
* fork tcount 程式，將文字傳入，取得關鍵詞的出現次數。
* 回傳結果至前端。

## tcount.c
* 使用 chaining 的 hashtable。
* 將 term 存入 hashtable 裡，用 ngram 方式對文章做正向最大匹配斷詞，之後去 hashtable 找看看有沒有這個詞，如果有的話就將次數加一。
