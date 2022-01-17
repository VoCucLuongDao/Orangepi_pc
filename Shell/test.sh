C1=3
echo "$C1"
C2="5
echo "$C2"
C3:=$(C1)
echo "$C3"
C3:=$(C2)
echo "$C3"
EOF
