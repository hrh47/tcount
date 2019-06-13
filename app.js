var path = require('path');
var htmlToText = require('html-to-text');
var read = require('node-readability');
var exec = require('child_process').exec;
var fs = require('fs');
var express = require('express');

var app = express();

app.get('/tcount', function (req, res) {
  url = req.query.loc;
  read(url, function(err, article, meta) {
    var html = article.title + '<br>' + article.content;
    var text = htmlToText.fromString(html, {
      ignoreHref: true,
      ignoreImage: true
    });
    // console.log(text);
    article.close();
    fs.writeFile(path.join(__dirname, 'article'), text, function(err) {
      if (err) {
        console.log('Write Error');
      }
    });

    var child = exec('./tcount noun <article', function(err, stdout, stderr) {
      var termCount = [];
      var outputs = stdout.split('\n');
      for (output of outputs) {
        if (output.length > 0) {
          var tcount = output.split(' ');
          tcount[1] = parseInt(tcount[1]);
          termCount.push(tcount);
        }
      }
      // console.log(termCount);
      res.send(termCount);
    });

  });

});

app.listen(3000, function () {
  console.log('Example app listening on port 3000!');
});
