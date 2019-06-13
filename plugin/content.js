
/**
 * Get the current URL.
 *
 * @param {function(string)} callback called when the URL of the current tab
 *   is found.
 */
function getCurrentTabUrl(callback) {
  // Query filter to be passed to chrome.tabs.query - see
  // https://developer.chrome.com/extensions/tabs#method-query
  var queryInfo = {
    active: true,
    currentWindow: true
  };

  chrome.tabs.query(queryInfo, (tabs) => {
    // chrome.tabs.query invokes the callback with a list of tabs that match the
    // query. When the popup is opened, there is certainly a window and at least
    // one tab, so we can safely assume that |tabs| is a non-empty array.
    // A window can only have one active tab at a time, so the array consists of
    // exactly one tab.
    var tab = tabs[0];

    // A tab is a plain object that provides information about the tab.
    // See https://developer.chrome.com/extensions/tabs#type-Tab
    var url = tab.url;

    // tab.url is only available if the "activeTab" permission is declared.
    // If you want to see the URL of other tabs (e.g. after removing active:true
    // from |queryInfo|), then the "tabs" permission is required to see their
    // "url" properties.
    console.assert(typeof url == 'string', 'tab.url should be a string');

    callback(url);
  });
}

  // This extension loads the saved background color for the current tab if one
  // exists. The user can select a new background color from the dropdown for the
  // current page, and it will be saved as part of the extension's isolated
  // storage. The chrome.storage API is used for this purpose. This is different
  // from the window.localStorage API, which is synchronous and stores data bound
  // to a document's origin. Also, using chrome.storage.sync instead of
  // chrome.storage.local allows the extension data to be synced across multiple
  // user devices.
document.addEventListener('DOMContentLoaded', () => {
  getCurrentTabUrl((url) => {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "http://localhost:3000/tcount?loc=" + escape(url), true);
    xhr.onreadystatechange = function() {
      if (xhr.readyState == 4) {
        // innerText does not let the attacker inject HTML elements.
        // console.log(xhr.responseText);

        var termCount = JSON.parse(xhr.responseText);
        if (termCount.length == 0) {
          document.getElementsByTagName('h1')[0].innerText = '沒有找到關鍵詞';
          return;
        }
        var terms = [];
        var counts = [];

        for (t of termCount) {
          terms.push(t[0]);
          counts.push(t[1]);
        }

        terms = terms.slice(0, 10);
        counts = counts.slice(0, 10);
        console.log(terms);
        console.log(counts);

        var ctx = document.getElementById("myChart").getContext("2d");
        // var color = Chart.helpers.color;

        var top3Index = 3;

        for (var i = top3Index; i < counts.length; i++) {
          if (counts[i] >= counts[2]) {
            top3Index++;
          }
        }

        var backgroundColors = [];
        var borderColors = [];

        for (var i = 0; i < top3Index; i++) {
          backgroundColors.push("rgba(255, 159, 64, 0.2)");
          borderColors.push("rgb(255, 159, 64)")
        }
        for (var i = top3Index; i < 10; i++) {
          backgroundColors.push("rgba(54, 162, 235, 0.2)");
          borderColors.push("rgb(54, 162, 235)")
        }

        var barChartData = {
            'labels': terms,
            'datasets': [{
              'label': '出現次數',
              'backgroundColor': backgroundColors,
              'borderColor': borderColors,
              'borderWidth': 1,
              'data': counts
            }]
        };

        var myBarChart = new Chart(ctx, {
            'type': 'bar',
            'data': barChartData,
            'options': {
              'scales': {
                'yAxes': [{
                  'ticks': {
                    'beginAtZero': true,
                    'callback': function(value, index, values) {
                        if (Math.floor(value) === value) {
                            return value;
                        }
                    }
                  }
                }]
              }
            }
        });

      }
    }
    xhr.send();
  });
});
