#!/bin/sh
#Author: patrick wuchunhuan@gmail.com

num2ip() {
    N=$1
    H1=$(($N & 0x000000ff))
    H2=$((($N & 0x0000ff00) >> 8))
    L1=$((($N & 0x00ff0000) >> 16))
    L2=$((($N & 0xff000000) >> 24))
    echo $L2.$L1.$H2.$H1
}

CheckIPAddr()
{
    if [ $# -ne 1 ]
    then
        return 1
    fi

    echo $1 | grep "^[0-9]\{1,3\}\.\([0-9]\{1,3\}\.\)\{2\}[0-9]\{1,3\}$" > /dev/null;
    #IP地址必须为全数字
    if [ $? -ne 0 ]
    then
        return 1
    fi
    ipaddr=$1
    a=`echo $ipaddr | awk -F . '{print $1}'`   #以"."分隔，取出每个列的值
    b=`echo $ipaddr | awk -F . '{print $2}'`
    c=`echo $ipaddr | awk -F . '{print $3}'`
    d=`echo $ipaddr | awk -F . '{print $4}'`
    for num in $a $b $c $d
    do
        if [ $num -gt 255 ] || [ $num -lt 0 ]     #每个数值必须在0-255之间
        then
            return 1
        fi
    done
        return 0
}

ip2num() {
    if [ $# -ne 1 ]           #判断传参数量
    then
        echo "Usage: $0 ipaddress."
        return 1
    fi

    IP_ADDR=$1

    CheckIPAddr $IP_ADDR
    if [ $? -eq 1 ]
    then
        echo "ip:$IP_ADDR is invalid."
        return 1
    fi

    IP_LIST=${IP_ADDR//./ };

    # 把点分十进制地址拆成数组(read的-a选项表示把输入读入到数组， 下标从0开始)
    read -a IP_ARRAY <<<${IP_LIST};

    # bash的$(()) 支持位运算
    echo $(( ${IP_ARRAY[0]}<<24 | ${IP_ARRAY[1]}<<16 | ${IP_ARRAY[2]}<<8 | ${IP_ARRAY[3]} ));

    # 这里演示另外一种不使用位运算的方法
    HEX_STRING=$(printf "0X%02X%02X%02X%02X\n" ${IP_ARRAY[0]} ${IP_ARRAY[1]} ${IP_ARRAY[2]} ${IP_ARRAY[3]});
    printf "%d\n" ${HEX_STRING};
    return 0
}

maskconvert() {
    # 把掩码长度转换成掩码
    # 255.255.255.255的整型值是4294967295
    if [ $# -ne 1 ]           #判断传参数量
    then
        echo "Usage: $0 mask_number eg. $0 24."
        return 1
    fi

    declare -i FULL_MASK_INT=4294967295
    declare -i MASK_LEN=$1
    declare -i LEFT_MOVE="32 - ${MASK_LEN}"
    declare -i N="${FULL_MASK_INT} << ${LEFT_MOVE}"
    declare -i H1="$N & 0x000000ff"
    declare -i H2="($N & 0x0000ff00) >> 8"
    declare -i L1="($N & 0x00ff0000) >> 16"
    declare -i L2="($N & 0xff000000) >> 24"
    echo "$L2.$L1.$H2.$H1"
}