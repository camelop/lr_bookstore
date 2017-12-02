#coding=utf-8
import os
exe = 'bookstore'

temp_dir = 'temp'

test_name = [
    'user test 1',
    'user test 2',
    'user test 3',
    'book test 1',
    'book test 2',
    'book test 3',
]
test_loc = list(map(lambda x: 'testcase'+str(x),range(1,7)))

for i in range(6):
    k = test_name[i]
    loc = test_loc[i]
    os.system('mkdir '+temp_dir)
    os.system('cp '+exe+' '+temp_dir+'/bookstore')
    command = 'cp '+'test_data/'+loc+'.txt '+temp_dir+'/command.txt;cd '+\
        temp_dir+';./'+'bookstore > ans.txt 2> cerr.txt;cd ..'
    os.system(command)
    result = os.system('cmp test_data/'+loc+'_ans.txt '+temp_dir+'/ans.txt')
    if result != 0:
        exit(1)
    os.system('rm -r '+temp_dir)
    print k,":\t-\033[1;32mACCEPT\033[0m-"
