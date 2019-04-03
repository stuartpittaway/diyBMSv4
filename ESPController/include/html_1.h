const char FILE_INDEX_HTML[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<meta name = "viewport" content = "width=device-width,initial-scale=1.0">
<title>DIY BMS CONTROLLER v4</title>
<!-- jquery-3.3.1.min.js -->
<script src="jquery.js" integrity="sha384-tsQFqpEReu7ZLhBV2VZlAu7zcOV+rXbYlF2cqB8txI/8aZajjp4Bqd+V6D5IgvKT" crossorigin="anonymous"></script>
<script src="echarts.simple.min.js" integrity="" crossorigin="anonymous"></script>
<style>
*{box-sizing:border-box}
body{margin:0;font-family:Arial,Helvetica,sans-serif;}
.header{overflow:hidden;background-color:#d4f0fb;padding:20px 10px}
.header a{float:left;color:#000;text-align:center;padding:12px;text-decoration:none;font-size:18px;line-height:25px;border-radius:4px}
.header a.logo{font-size:25px;font-weight:700}
.header a:hover{background-color:#ddd;color:#000}
.header a.active{background-color:#1e90ff;color:#fff}
.header-right{float:right}
.logo { float:left; background-image: url("logo.gif");  width:191px;  height:48px;}

.info {width:100%;border:solid 2px gray;padding:0.1em;}
.info .stat {display:inline-block;width:20%}

.info .stat .t {font-size:small;display:block;width:100%;text-align:right;}
.info .stat .v {font-size:large;display:block;width:100%;text-align:right;}
#refreshbar { width:100%; padding:0; margin:0; height:4px; background-color:#d3f9fa;}

.ct-series-a .ct-bar {
  stroke: purple;
  stroke-width: 25px;
}

@media screen and (max-width:500px){
  .header a,.header-right{float:none}
  .header a{display:block;text-align:left}
}

@media screen and (max-width:640px){

}

.error:before { content: '!'; }
.error { color:#D8000C;background-color:#FFBABA; padding:10px;display:none; width:100%;}

</style>
</head>
<body>
<div class="header">
  <div class="logo">&nbsp;</div>
  <div class="header-right">
    <a class="active" href="#home">Home</a>
    <a href="#contact">Settings</a>
    <a href="#about">About</a>
  </div>
</div>
<div id='refreshbar'></div>
<div id='commserr' class='error'>The controller is having difficulty communicating with the cell monitoring modules.</div>
<div id='iperror' class='error'>Cannot communicate with the controller for status updates.</div>

<div id="info" class="info">
<div class="stat"><span class="t">Voltage:</span><span class="v" id="voltage"></span></div>
<div class="stat"><span class="t">Current:</span><span class="v" id="current"></span></div>
<div class="stat"><span class="t">Packet Errors:</span><span class="v" id="badpkt"></span></div>
<div class="stat"><span class="t">CRC Errors:</span><span class="v" id="badcrc"></span></div>
</div>

<div class="" style="height:768px;width:100%;">
<div id="graph1" style="width:100%; height:100%;"></div>
</div>
<script type="text/javascript">
var g1=null;
var g2=null;
var g3=null;

function queryBMS() {
  $.getJSON( "monitor.json", function( jsondata ) {

    var voltage=0.0;
    var labels=[];
    var voltages=[];
    var voltagesmin=[];
    var voltagesmax=[];
    var tempint=[];
    var tempext=[];
    $.each(jsondata.monitor.cells, function( index, value ) {
      voltages.push((parseFloat(value.v)/1000.0));

      voltagesmin.push((parseFloat(value.minv)/1000.0));
      voltagesmax.push((parseFloat(value.maxv)/1000.0));

      labels.push(index);

      tempint.push(value.int);
      tempext.push(value.ext==-40 ? 0:value.ext  );

      //TODO: This needs to be voltage per bank not total
      voltage+=(parseFloat(value.v)/1000.0);
    });

    if (g1==null) {
      // based on prepared DOM, initialize echarts instance
      g1 = echarts.init(document.getElementById('graph1'));

      var labelOption = {
          normal: {
              show: true,
              position: 'insideBottom',
              distance: 15,
              align: 'left',
              verticalAlign: 'middle',
              rotate: 90,
              formatter: '{c}V',
              fontSize: 20
          }
      };

      var labelOption1 = {
          normal: {
              show: true,
              position: 'end',
              formatter: '{c}V',
              fontSize: 20
          }
      };


            var labelOption2 = {
                normal: {
                    show: true,
                    position: 'insideBottom',
                    distance: 15,
                    align: 'left',
                    verticalAlign: 'middle',
                    rotate: 90,
                    formatter: '{c}°C',
                    fontSize: 20
                }
            };
      // specify chart configuration item and data
      var option = {
        color: ['#003366', '#006699', '#4cabce'],
        tooltip: { trigger: 'axis', axisPointer: { type: 'cross', crossStyle: { color: '#999' } } },
          legend: { data:['Voltage'] },
          xAxis: [{gridIndex: 0,type:'category' },{  gridIndex: 1,type:'category' }],
          yAxis: [
            {gridIndex: 0,name:'Volts',type:'value',min:2.5,max:5.0,interval:0.25,position:'left', axisLabel: { formatter: '{value}V' }}
            ,{gridIndex: 1,name:'Temperature',type:'value',interval:10,position:'right'
            ,axisLabel:{ formatter: '{value} °C' }
            ,axisLine:{show:false, lineStyle:{type:'dotted'} } } ]
          ,series: [{ name: 'Voltage', type: 'bar', data: [], label: labelOption }
              ,{ name: 'Min V', type: 'line', data: [], label: labelOption1 }
              ,{ name: 'Max V', type: 'line', data: [], label: labelOption1 }
              ,{xAxisIndex:1, yAxisIndex:1, name:'BypassTemperature',type:'bar', data: [], label: labelOption2 }
              ,{xAxisIndex:1, yAxisIndex:1, name:'CellTemperature',type:'bar',data: [], label: labelOption2 }
          ],
          grid: [{containLabel:false,left:'5%',right:'5%',bottom:'32%'},{containLabel:false, left:'5%',right:'5%',top:'78%'}],
      };



      // use configuration item and data specified to show chart
      g1.setOption(option);

    } else {
      g1.setOption({
          xAxis: { data: labels },
          series: [{ name: 'Voltage', data: voltages }
          ,{ name: 'Min V', data: voltagesmin }
          ,{ name: 'Max V', data: voltagesmax }
          ,{ name: 'BypassTemperature', data: tempint }
          ,{ name: 'CellTemperature', data: tempext }]
      });
    }

    $("#badcrc").html(jsondata.monitor.badcrc);
    $("#badpkt").html(jsondata.monitor.badpkt);

    $("#voltage").html(voltage.toFixed(2));

    $("#current").html("");

    if (jsondata.monitor.commserr==true) {
      $("#commserr").show();
    } else {
      $("#commserr").fadeOut();
    }

    $("#iperror").hide();

  }).fail(function() {
     $("#iperror").show();
  });
}

//var refreshcounter=1;

function countdown() {
  queryBMS();
  $("#refreshbar").width('100%').animate({ width: '-=100%' },{ duration:2500, complete: countdown, queue:false});
}

$(function() {
  countdown();
  //setTimeout(countdown,1000);

  //$("#refreshbar").animate({ width: '-=100%', done: countdown }, 5000);

});
</script>
</body>
</html>
)=====";
