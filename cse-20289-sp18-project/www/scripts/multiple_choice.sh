#!/bin/sh

export PATH=$PATH:/afs/nd.edu/user15/pbui/pub/bin

echo "HTTP/1.0 200 OK"
echo "Content-type: text/html"
echo

cat <<EOF
<h1>TEST</h1>
<hr>
<body>
<form>
1. What is the best script in the world?
<select id="mySelect1">
<option value="YES">Cowsay</option>
<option value="WRONG">Thor</option>
<option value="WRONG">Spidey</option>
</select>
<button type="button" onclick="myFunction1()">Submit</button>

<p id="demo1"></p>

<script>
function myFunction1() {
var x = document.getElementById("mySelect1").value;
document.getElementById("demo1").innerHTML = x;
}
</script>
</form>

<form>
2. How would you display the contents of a directory?
<select id="mySelect2">
<option value="WRONG">pwd</option>
<option value="YES">ls</option>
<option value="WRONG">cd</option>
</select>
<button type="button" onclick="myFunction2()">Submit</button>

<p id="demo2"></p>

<script>
function myFunction2() {
var y = document.getElementById("mySelect2").value;
document.getElementById("demo2").innerHTML = y;
}
</script>

</form>
</body>
<hr>
EOF
