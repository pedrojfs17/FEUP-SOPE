./test -q 1 -u 1 -f fifo.server | tee -a result.log
echo "----------------------------"
./test -q 2 -u 3 -f fifo.server | tee -a result.log
echo "----------------------------"
./test -q 5 -u 5 -f fifo.server | tee -a result.log
echo "----------------------------"
./test -q 2 -u 5 -f fifo.server | tee -a result.log
echo "----------------------------"
./test -q 10 -u 10 -f fifo.server | tee -a result.log
echo "----------------------------"
./test -q 10 -u 5 -f fifo.server | tee -a result.log
echo "----------------------------"
./test -q 15 -u 10 -f fifo.server | tee -a result.log
echo "----------------------------"
./test -q 30 -u 35 -f fifo.server | tee -a result.log
echo "----------------------------"
./test -q 70 -u 80 -f fifo.server | tee -a result.log
echo "----------------------------"