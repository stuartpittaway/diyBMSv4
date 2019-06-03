const char FILE_STYLE_CSS[] PROGMEM = R"=====(
*{box-sizing:border-box}
body{margin:0;font-family:Arial,Helvetica,sans-serif;}
.header{overflow:hidden;background-color:#d4f0fb;padding:20px 10px}
.header a{float:left;color:#000;text-align:center;padding:12px;text-decoration:none;font-size:18px;line-height:25px;border-radius:4px}
.header a.logo{font-size:25px;font-weight:700}
.header a:hover{background-color:#ddd;color:#000}
.header a.active{background-color:#1e90ff;color:#fff}
.header-right{float:right}

.logocontainer{float:left;width:191px;height:52px;}
.info {width:100%;padding:4px;}
.info .stat {display:block;border:solid 2px gray;padding:2px;margin:2px;float:left;min-width:145px;}
.info .stat .x {display:block;width:100%;text-align:right;}
.info .stat .t {font-size:10pt;color:darkgray;}
.info .stat .v {font-size:22pt;font-family: 'Nova Mono', monospace;font-weight:bold;color:#1e90ff;}
#refreshbar { width:100%; padding:0; margin:0; height:4px; background-color:#D3D3D3;}
.graphs {height:768px;width:100%;}
.ct-series-a .ct-bar {stroke:purple;stroke-width:25px;}
form > div > div {padding-bottom:8px;}
form label {width:220px;display:inline-block;text-align:right;}
form input {width:100px;display:inline-block;padding-left:10px;padding-right:10px;margin-left:10px;font-family:'Nova Mono',monospace;}
form input[type='input'] {width:225px;}
form input[type='submit'] {font-family:Arial,Helvetica,sans-serif;border:none;color:white;background-color:#1e90ff;text-align:center;text-decoration:none;display:inline-block;font-size:14px;width:150pt;padding:10px;}

.error { color:#D8000C;background-color:#FFBABA; padding:10px;display:none; width:100%;}
.success { color:#000000;background-color:#49BE3B; padding:10px;display:none; width:100%;}
.page {clear:both; width:100%; display:none;margin:5px;}

h2 { margin:0; }
.page .region {max-width:600px;float:left;padding:10px;margin-right:10px;width:45%;}

#modulesTable span {display:inline-block;}
#modulesTable .rows span {min-width:90px;font-family: 'Nova Mono', monospace;text-align:right;}
#modulesTable .rows button {margin-left:20px;}
#modulesTable .th span {text-align:right;min-width:90px}
#modulesTable .selected {background-color:#d3f9fa;}

@media screen and (max-width:640px){
  .page .region {width:100%;}
  .header a {float:none;padding:6px;font-size:14px;line-height:20px;}
  .graphs {height:480px;width:95%;}
  .info .stat .v { font-size:12pt;}
  .info .stat { min-width:85px;min-height:75px;}
  form label {width:160px;}
  form input[type='input'] {width:175px;}

  #modulesTable .hide {display:none;}
  #modulesTable .rows span {min-width:65px;}
  #modulesTable .rows button {margin-left:2px;}
  #modulesTable .th span {min-width:65px}

  .logo{width:75px;height:20px;}
  .logocontainer {width:80px;height:28px;}
}

)=====";
