#include <linux/module.h>
#include <net/sock.h> 
#include <linux/netlink.h>
#include <linux/skbuff.h> 
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>
#define NETLINK_USER 31

struct sock *nl_sk = NULL;
int pid;
//int s_pid;
//int s_pid2;
struct mystruct	{
        char key[9];
	int data;
	struct list_head mylist; 
        struct rhash_head linkage;
};
struct list_head s_head;
struct list_head p_head;



static void hello_nl_recv_msg(struct sk_buff *skb)
{
	
    	struct nlmsghdr *nlh;
	struct sk_buff *skb_out;
	int msg_size;
	//char *msg = "Hello from kernel";
         char *msg;
	int res;
        int p_second = 0;
	int s_second = 0;
	struct mystruct* s_node = kmalloc(sizeof(struct mystruct*),GFP_KERNEL);
	struct mystruct* p_node = kmalloc(sizeof(struct mystruct*),GFP_KERNEL);
        struct mystruct* s_entry = kmalloc(sizeof(struct mystruct*),GFP_KERNEL);
	struct mystruct* p_entry = kmalloc(sizeof(struct mystruct*),GFP_KERNEL);
	const static struct rhashtable_params object_params = {
	.key_len     = sizeof(9*char),
	.key_offset  = offsetof(struct object, key),
	.head_offset = offsetof(struct object, linkage),
    };

    struct rhashtable my_objects;

    success = rhashtable_init(&my_objects, &object_params);
     

	printk(KERN_INFO "Entering: %s\n", __FUNCTION__);

    	//msg_size = strlen(msg);

    	nlh = (struct nlmsghdr *)skb->data;
    	printk(KERN_INFO "Netlink received msg payload:%s\n", (char *)nlmsg_data(nlh));
	msg =nlmsg_data(nlh);
        msg_size = strlen(msg);
        printk(KERN_INFO "The msg is P or not : %d\n", strcmp(msg,"P"));
        
          if(msg[0]=='S'&& s_second ==0){
            s_node ->data = nlh->nlmsg_pid;
	    for(int i=0; i<7; i++){
            s_node->key[i]= msg[i+1];
}
            s_node->key[8] = 0;
            list_add(&s_node ->mylist , &s_head);
            s_second = 1;
           //if(!second){
	//s_pid = nlh->nlmsg_pid; /*pid of publishing process */
        //second = 1;
        printk(KERN_INFO "The subscriber PID is saved\n");//}
//else{
   // s_pid2 = nlh->nlmsg_pid; /*pid of publishing process */
       // printk(KERN_INFO "The subscriber PID is saved\n");
//}
}
         else if(msg[0]=='P'&& p_second ==0){
            p_node ->data = nlh->nlmsg_pid;
	for(int i=0; i<7; i++){
            p_node->key[i]= msg[i+1];
}
            p_node->key[8] = 0;
            list_add(&p_node ->mylist , &p_head);
	     p_second = 1;
	//pid = nlh->nlmsg_pid; /*pid of publishing process */
            
        printk(KERN_INFO "The publisher PID is saved\n");
}

list_for_each_entry(s_entry,&s_head,mylist){
        if(pid == nlh->nlmsg_pid && strcmp(msg,"P")!=0){
        printk(KERN_INFO "The publisher has sent new data to kernel\n");
	skb_out = nlmsg_new(msg_size, 0);
	if (!skb_out) {
		printk(KERN_ERR "Failed to allocate new skb\n");
	      	return;
	}

	nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
	//NETLINK_CB(skb_out).group = 1<<3;
        // NETLINK_CB(skb_out).dst_group = 0;
	//NETLINK_CB(skb).groups = 1;
 	//NETLINK_CB(skb).pid = 0;
	NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
	strncpy(nlmsg_data(nlh), msg, msg_size);
           res = nlmsg_unicast(nl_sk, skb_out,s_entry->data);
	//res = nlmsg_multicast(nl_sk, skb_out,s_pid,1,GFP_KERNEL);
	//res = nlmsg_multicast(nl_sk, skb_out,s_pid2,1<<3,GFP_KERNEL);
       printk(KERN_INFO "The message from publisher is able to sent back to Subscriber\n");
	if (res < 0)
		printk(KERN_INFO "Error while sending bak to user\n");
}
}
}

static int __init hello_init(void)
{
	INIT_LIST_HEAD(&s_head);
	INIT_LIST_HEAD(&p_head);
        
    printk("Entering: %s\n", __FUNCTION__);
    struct netlink_kernel_cfg cfg = {
        .input = hello_nl_recv_msg,
    };

    nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
    if (!nl_sk) {
        printk(KERN_ALERT "Error creating socket.\n");
        return -10;
    }

    return 0;
}

static void __exit hello_exit(void)
{

    printk(KERN_INFO "exiting hello module\n");
    netlink_kernel_release(nl_sk);
}

module_init(hello_init); module_exit(hello_exit);

MODULE_LICENSE("GPL");
