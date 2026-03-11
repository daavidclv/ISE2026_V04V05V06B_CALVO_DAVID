t <html><head><title>Control RTC</title>
t <style>
t  body{font-family:sans-serif;background:#f4f8fb;color:#333;text-align:center}
t  .c{background:#fff;border-top:5px solid #0056b3;padding:20px;margin:20px auto;max-width:400px}
t  .v{font-size:28px;font-weight:bold;color:#0056b3;display:block;margin:10px 0}
t  .btn{background:#0056b3;color:#fff;border:none;padding:8px 15px;margin:5px;cursor:pointer}
t </style>
t <script language=JavaScript>
t  var a=1;
t  setInterval(function(){if(a)location.reload();},1000);
t  function tgl(){
t   a=!a;
t   document.getElementById('m').innerHTML='Modo: '+(a?'Auto':'Manual');
t  }
t </script></head>
i pg_header.inc
t <h2 align=center><br>Panel de Control RTC</h2>
t <div class="c">
t  <div id="m" style="color:#666">Modo: Auto</div><hr>
t  <p style="margin:0">Fecha del Sistema:</p>
c h 1  <span class="v">%s</span>
t  <p style="margin:0">Hora Local:</p>
c h 2  <span class="v">%s</span>
t  <hr>
t  <button class="btn" onclick="location.reload()">Actualizar ahora</button>
t  <button class="btn" onclick="tgl()">Cambiar modo</button>
t </div>
i pg_footer.inc
. End of script must be closed with period.