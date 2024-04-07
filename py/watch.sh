
while true;
#python3 -c "import tty; tty.setcbreak(1)"
do
	echo -e "\e[0;0H"
	img2sixel ../frame.png
	echo
	sleep 0.1
done

