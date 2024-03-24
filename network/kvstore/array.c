/*
 * 本代码实现kv存储中的array数组存储结构
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"array.h"


/*-----------------------------函数声明------------------------------*/
// array遍历查找
// 参数说明：
//  cur_blk：传一个没有定义的块！用于返回当前kv存储对所在的块
// 返回值：正常返回键值对，NULL表示没有
kv_array_item_t * kv_array_search(kv_array_t* kv_a, const char* key, kv_array_block_t* cur_blk);// 在最后创建一个块



// 返回值：正常返回最后一个块，NULL表示创建失败
kv_array_block_t* kv_array_create_block(kv_array_t* kv_a);


// 释放给定的块
// 返回值：0成功，-1失败
int kv_array_free_block(kv_array_t* kv_a, kv_array_block_t* blk);



// 找到第一个空节点，若全满了就创建一个新的块
// 参数说明：
//  kv_a：array数据类型的头
//  cur_blk：传一个没有定义的块！用于返回当前kv存储对所在的块
kv_array_item_t * kv_array_find_null(kv_array_t* kv_a, kv_array_block_t* cur_blk);



