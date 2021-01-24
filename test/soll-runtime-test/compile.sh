SOLL_BASE="../.."
SOLL=$SOLL_BASE/build/tools/soll/soll
YULS=`find $SOLL_BASE/test -name *.yul`
RED='\033[0;31m'
NC='\033[0m'

for i in $YULS; do
	echo "compile $i"
	base=${i%.yul}
	basedir=`dirname $i`
	if $SOLL -lang=Yul $i 2> /dev/null; then
		[ -f $basedir/object.wasm ] && cp $basedir/object.wasm ${base}.wasm
	else
		echo -e "${RED}compile failed $i${NC}"
	fi
done

exit 0

