#include "hal/clk.h"
#include <acpi/acpi.h>
#include <apic.h>
#include <apic_timer.h>
#include <asm.h>
#include <cls.h>
#include <decls.h>
#include <gdt.h>
#include <hal/hal.h>
#include <hpet.h>
#include <interrupts.h>
#include <irq.h>
#include <libk/kgfx.h>
#include <libk/kio.h>
#include <libk/macros.h>
#include <libk/queue.h>
#include <libk/rbtree.h>
#include <libk/vga_kgfx.h>
#include <mem/memory.h>
#include <mem/pimemory.h>
#include <mem/vimemory.h>
#include <multiboot.h>
#include <pic.h>
#include <stddef.h>
#include <stdint.h>
#include <threading.h>
#include <threading/pcb.h>
#include <threading/tcb.h>
#include <threading/threading.h>
#include <threading/userspace.h>
#include <x86_64.h>

extern void kernel_main(void);
extern void change_stacks(void);

// Code thanks to nullplan from the osdev wiki
// https://forum.osdev.org/viewtopic.php?t=57103 This code calls the global
// constructors
#pragma region Global Constructors
typedef void initfunc_t(void);
extern initfunc_t *__init_array_start[], *__init_array_end[];

static void handle_init_array(void) {
  size_t nfunc = ((uintptr_t)__init_array_end - (uintptr_t)__init_array_start) /
                 sizeof(initfunc_t *);
  for (initfunc_t **p = __init_array_start; p < __init_array_start + nfunc; p++)
    (*p)();
}
#pragma endregion

typedef struct {
  uint32_t total_size;
  uint32_t reserved;
} PACKED multiboot_header_t;

typedef struct {
  uint32_t type;
  uint32_t size;
} PACKED multiboot_tag_t;

void kernel_secondary_start(void);

void create_local_proccess(void) {
  PCB_t *pcb = create_process();
  TCB_t *tcb = create_thread(pcb, NULL);
  tcb->priority = TP_NORMAL;

  tcb->state = THREAD_RUNNING;

#define FS_MSR 0xC0000100

  wrmsr(FS_MSR, (size_t)tcb);
}

extern size_t feature_flags;

void kernel_start(uint8_t *multiboot) {

  //  test_print(multiboot);

  handle_init_array();
  init_multiboot(multiboot);

  init_memory_manager();
  kgfx_init();
  kgfx_clear();

  kio_printf("Initialized Memory Manager\n");

  init_global_brk();
  kio_printf("Initialized Global Heap (brk)\n");

  init_cls(feature_flags);
  kio_printf("Initialized CLS (Core Local Storage)\n");

  create_gdt();
  kio_printf("Created the GDT\n");

  // // kio_clear();
  //
  // size_t phys_1 = (size_t)phys_alloc();
  // phys_free((void *)phys_1);
  // size_t phys_2 = (size_t)phys_alloc();
  // size_t phys_3 = (size_t)phys_alloc();
  // phys_free((void *)phys_2);
  // size_t phys_4 = (size_t)phys_alloc();
  // phys_free((void *)phys_3);
  // phys_free((void *)phys_4);
  //
  // extern char ex0nd_kernel[];
  //
  // kio_printf("Addr 1 %x, 2 %x, 3 %x, 4 %x diff %x\n", phys_1, phys_2, phys_3,
  //            phys_4, (size_t)(void *)end_kernel - KERNEL_CODE_OFFSET -
  //            phys_1);
  //
  // uint64_t *page = map_page((void *)GB, PT_READ_WRITE, 0);
  // *page = 48;
  // map_page((void *)(GB + PAGE_SIZE), PT_READ_WRITE, 1);
  //
  // kio_printf("Addr %x, value %x\n", (size_t)page, *page);
  // unmap_page((void *)GB);
  // unmap_page((void *)(GB + PAGE_SIZE));
  //
  // vmm_kernel_region_t region;
  // create_kernel_region(&region);
  //
  // increment_kernel_brk(&region, 0x4001);
  // uint64_t *num = increment_kernel_brk(&region, 0);
  // *num = 49;
  //
  // kio_printf("Num %u\n", *num);
  //
  // decrement_kernel_brk(&region, 0x4001);

  // Set up default kernel region. during smp startup each core should make
  // their kernel region
  create_local_proccess();

  change_stacks();
}

queue_t queue = {.head = NULL, .tail = NULL};

void test_queue(void) {
  kio_printf("\nLock Free Queue Tests\n\n");

  // Empty insert test
  kio_printf("QUEUE EMPTY INSERT TEST ");
  queue_node_t *node1 = gmalloc(sizeof(queue_node_t));
  queue_enqueue(&queue, node1);

  if (queue.head == queue.tail && queue.head == node1) {
    kio_printf("[PASSED]\n");
  } else {
    kio_printf("[FAILED]\n");
  }

  // Dequeue test
  kio_printf("QUEUE DEQUEUE TEST ");

  queue_node_t *node2 = gmalloc(sizeof(queue_node_t));
  queue_enqueue(&queue, node2);

  queue_node_t *pop1 = queue_dequeue(&queue);

  if (pop1 == node1) {
    kio_printf("[PASSED]\n");
  } else {
    kio_printf("[FAILED]\n");
  }

  gfree(node1);
  node1 = NULL;
  pop1 = NULL;

  // Consecutive element dequeue test
  kio_printf("QUEUE CONSECUTIVE ELEMENT DEQUEUE TEST ");

  queue_node_t *pop2 = queue_dequeue(&queue);

  if (pop2 == node2) {
    kio_printf("[PASSED]\n");
  } else {
    kio_printf("[FAILED]\n");
  }

  gfree(node2);
  node2 = NULL;
  pop2 = NULL;

  // Single element dequeue test
  kio_printf("QUEUE SINGLE ELEMENT DEQUEUE TEST ");

  queue_node_t *node3 = gmalloc(sizeof(queue_node_t));
  queue_enqueue(&queue, node3);

  queue_node_t *pop3 = queue_dequeue(&queue);

  if (pop3 == node3) {
    kio_printf("[PASSED]\n");
  } else {
    kio_printf("[FAILED]\n");
  }

  gfree(node3);
  node3 = NULL;
  pop3 = NULL;

  // Empty dequeue test
  kio_printf("QUEUE EMPTY DEQUEUE TEST ");

  queue_node_t *pop4 = queue_dequeue(&queue);

  if (pop4 == NULL) {
    kio_printf("[PASSED]\n");
  } else {
    kio_printf("[FAILED]\n");
  }
}

bool failed = false;

size_t verify_rbnode(rbtree_t *tree, rbnode_t *node) {
  if ((node->left == NULL || node->right == NULL) && node != &tree->nil) {
    failed = true;
    return 0;
  }

  if (node->color == RB_RED) {
    if (node->left->color == RB_RED || node->right->color == RB_RED) {
      failed = true;
      kio_printf("Here\n");
    }
  }

  size_t left_height = 0;
  if (node != &tree->nil) {
    left_height = verify_rbnode(tree, node->left);
  }
  size_t right_height = 0;
  if (node != &tree->nil) {
    right_height = verify_rbnode(tree, node->right);
  }

  if (left_height == right_height) {
    if (node->color == RB_BLACK)
      return left_height + 1;
    return left_height;
  }

  failed = true;
  return left_height + 1;
}

bool verify_rbtree(rbtree_t *tree) {
  if (tree->root == NULL)
    return true;

  verify_rbnode(tree, tree->root);

  return !failed;
}

void print_node(rbtree_t *tree, rbnode_t *node, size_t indent) {
  if (node == &tree->nil) {
    return;
  }

  for (size_t i = 0; i < indent; i++) {
    kio_printf("\t");
  }

  if (node->color == RB_RED) {
    kio_printf("RED %u\n", node->value);
  } else {
    kio_printf("BLACK %u\n", node->value);
  }

  print_node(tree, node->left, indent + 1);
  if (node->left == &tree->nil && node->right != &tree->nil) {
    for (size_t i = 0; i < indent + 1; i++) {
      kio_printf("\t");
    }
    kio_printf("BLACK NIL\n");
  }

  print_node(tree, node->right, indent + 1);
  if (node->right == &tree->nil && node->left != &tree->nil) {
    for (size_t i = 0; i < indent + 1; i++) {
      kio_printf("\t");
    }
    kio_printf("BLACK NIL\n");
  }
}

void print_tree(rbtree_t *tree) {
  if (tree->root != NULL) {
    print_node(tree, tree->root, 0);
  } else {
    kio_printf("NULL TREE\n");
  }
}

void test_rbtree(void) {
  rbtree_t tree;
  rb_create(&tree);

  kio_printf("\nRed Black Tree Tests\n\n");

  // Empty insert
  kio_printf("RBTREE INSERT EMPTY INSERT ");

  rbnode_t *node1 = gmalloc(sizeof(rbnode_t));
  node1->value = 3920;
  rb_insert(&tree, node1);

  if (verify_rbtree(&tree)) {
    kio_printf("[PASSED]\n");
  } else {
    kio_printf("[FAILED]\n");
  }

  // Case 3: Root parent
  kio_printf("RBTREE INSERT ROOT PARENT ");

  rbnode_t *node2 = gmalloc(sizeof(rbnode_t));
  node2->value = 4394949;
  rb_insert(&tree, node2);

  rbnode_t *node3 = gmalloc(sizeof(rbnode_t));
  node3->value = 439;
  rb_insert(&tree, node3);

  if (verify_rbtree(&tree)) {
    kio_printf("[PASSED]\n");
  } else {
    kio_printf("[FAILED]\n");
  }

  // Case 4: Red parent, Root grandparent
  kio_printf("RBTREE INSERT RED PARENT ROOT GRANDPARENT ");

  rbnode_t *node4 = gmalloc(sizeof(rbnode_t));
  node4->value = 100;
  rb_insert(&tree, node4);

  if (verify_rbtree(&tree)) {
    kio_printf("[PASSED]\n");
  } else {
    kio_printf("[FAILED]\n");
  }

  rbnode_t *node5 = gmalloc(sizeof(rbnode_t));
  node5->value = 1000;
  rb_insert(&tree, node5);

  // Case 2: Red Parent Black Grandparent Red Uncle
  kio_printf("RBTREE INSERT RED PARENT BLACK GRANDPARENT RED UNCLE ");

  rbnode_t *node6 = gmalloc(sizeof(rbnode_t));
  node6->value = 19;
  rb_insert(&tree, node6);

  if (verify_rbtree(&tree)) {
    kio_printf("[PASSED]\n");
  } else {
    kio_printf("[FAILED]\n");
  }

  // Case 1: Black Parent
  kio_printf("RBTREE INSERT BLACK PARENT ");

  rbnode_t *node7 = gmalloc(sizeof(rbnode_t));
  node7->value = 10;
  rb_insert(&tree, node7);

  if (verify_rbtree(&tree)) {
    kio_printf("[PASSED]\n");
  } else {
    kio_printf("[FAILED]\n");
  }

  // Case 6: Outer Red Parent Black Grandparent Black Uncle
  kio_printf("RBTREE INSERT OUTER RED PARENT BLACK GRANDPARENT RED UNCLE ");

  rbnode_t *node8 = gmalloc(sizeof(rbnode_t));
  node8->value = 140;
  rb_insert(&tree, node8);

  if (verify_rbtree(&tree)) {
    kio_printf("[PASSED]\n");
  } else {
    kio_printf("[FAILED]\n");
  }

  rbnode_t *node9 = gmalloc(sizeof(rbnode_t));
  node9->value = 1200;
  rb_insert(&tree, node9);

  // Case 5: Inner Red Parent Black Grandparent Black Uncle
  kio_printf("RBTREE INSERT INNER RED PARENT BLACK GRANDPARENT RED UNCLE ");

  rbnode_t *node10 = gmalloc(sizeof(rbnode_t));
  node10->value = 1100;
  rb_insert(&tree, node10);

  if (verify_rbtree(&tree)) {
    kio_printf("[PASSED]\n");
  } else {
    kio_printf("[FAILED]\n");
  }

  kio_printf("\n");

  // Red Node no Children
  kio_printf("RBTREE DELETE RED WITH NO KIDS ");

  rb_delete(&tree, node9);
  gfree(node9);

  if (verify_rbtree(&tree)) {
    kio_printf("[PASSED]\n");
  } else {
    kio_printf("[FAILED]\n");
  }

  // 1 Child
  kio_printf("RBTREE DELETE 1 CHILD ");

  rb_delete(&tree, node10);
  gfree(node10);

  if (verify_rbtree(&tree)) {
    kio_printf("[PASSED]\n");
  } else {
    kio_printf("[FAILED]\n");
  }

  // 2 Children
  kio_printf("RBTREE DELETE 2 CHILDREN ");

  rb_delete(&tree, node3);
  gfree(node3);

  if (verify_rbtree(&tree)) {
    kio_printf("[PASSED]\n");
  } else {
    kio_printf("[FAILED]\n");
  }

  // Case 6: Red Distant Black Sibling
  kio_printf("RBTREE DELETE RED DISTANT BLACK SIBLING ");

  rb_delete(&tree, node7);
  gfree(node7);

  if (verify_rbtree(&tree)) {
    kio_printf("[PASSED]\n");
  } else {
    kio_printf("[FAILED]\n");
  }

  // Case 4: Red Parent Black Sibling
  kio_printf("RBTREE DELETE RED PARENT BLACK SIBLING ");

  rb_delete(&tree, node8);
  gfree(node8);

  if (verify_rbtree(&tree)) {
    kio_printf("[PASSED]\n");
  } else {
    kio_printf("[FAILED]\n");
  }

  rbnode_t *node11 = gmalloc(sizeof(rbnode_t));
  node11->value = 2000;
  rb_insert(&tree, node11);

  rbnode_t *node12 = gmalloc(sizeof(rbnode_t));
  node12->value = 5;
  rb_insert(&tree, node12);

  rbnode_t *node13 = gmalloc(sizeof(rbnode_t));
  node13->value = 90;
  rb_insert(&tree, node13);

  // Case 5: Red CLose Nephew
  kio_printf("RBTREE DELETE RED CLOSE NEPHEW ");

  rb_delete(&tree, node12);
  gfree(node12);

  if (verify_rbtree(&tree)) {
    kio_printf("[PASSED]\n");
  } else {
    kio_printf("[FAILED]\n");
  }

  rbnode_t *node14 = gmalloc(sizeof(rbnode_t));
  node14->value = 3000;
  rb_insert(&tree, node14);

  rbnode_t *node15 = gmalloc(sizeof(rbnode_t));
  node15->value = 1500;
  rb_insert(&tree, node15);

  rb_delete(&tree, node11);
  gfree(node11);

  rb_delete(&tree, node2);
  gfree(node2);

  rb_delete(&tree, node15);
  gfree(node15);

  rb_delete(&tree, node1);
  gfree(node1);

  // Case 3: Red Sibling
  kio_printf("RBTREE DELETE RED SIBLING ");

  rb_delete(&tree, node13);
  gfree(node13);

  if (verify_rbtree(&tree)) {
    kio_printf("[PASSED]\n");
  } else {
    kio_printf("[FAILED]\n");
  }

  rb_delete(&tree, node6);
  gfree(node6);

  // Case 2: All Black
  kio_printf("RBTREE DELETE ALL BLACK ");

  rb_delete(&tree, node14);
  gfree(node14);

  if (verify_rbtree(&tree)) {
    kio_printf("[PASSED]\n");
  } else {
    kio_printf("[FAILED]\n");
  }

  rb_delete(&tree, node4);
  gfree(node4);

  // Case 1: Root
  kio_printf("RBTREE DELETE ROOT ");

  rb_delete(&tree, node5);
  gfree(node5);

  if (verify_rbtree(&tree)) {
    kio_printf("[PASSED]\n");
  } else {
    kio_printf("[FAILED]\n");
  }

  kio_printf("\n");
}

void kernel_secondary_start(void) {

  // Uncomment to make the kernel fault to show that moving the break backwards
  // unmaps the pages
  //*num = 49;
  //
  // kio_printf("Num %u\n", *num);

  // It would create interrupts otherwise
  disable_pic();
  init_interrupts();
  kio_printf("Initialized Interrupts\n");

  init_irq();
  kio_printf("Initialized IRQs\n");

  init_threading();
  kio_printf("Initialized Threading\n");

  init_apic_timer();
  kio_printf("Initialized APIC Timer\n");

  init_x86_64_hal();
  kio_printf("Initialized HAL\n");

  start_cores();
  kio_printf("Started all cores\n");

  // queue_node_t *node1 = gmalloc(sizeof(queue_node_t));
  // queue_node_t *node2 = gmalloc(sizeof(queue_node_t));
  // queue_node_t *node3 = gmalloc(sizeof(queue_node_t));

  // queue_enqueue(&queue, node1);
  // queue_enqueue(&queue, node2);

  // queue_node_t *pop1 = queue_dequeue(&queue);
  // kio_printf("Node 1 %x, %x is %x\n", (size_t)node1, (size_t)pop1,
  //            (size_t)(node1 == pop1));

  // queue_enqueue(&queue, node3);
  // queue_node_t *pop2 = queue_dequeue(&queue);
  // kio_printf("Node 2 %x, %x is %x\n", (size_t)node2, (size_t)pop2,
  //            (size_t)(node2 == pop2));
  // queue_node_t *pop3 = queue_dequeue(&queue);
  // kio_printf("Node 3 %x, %x is %x\n", (size_t)node3, (size_t)pop3,
  //            (size_t)(node3 == pop3));

  test_queue();
  test_rbtree();

  start_preemption();
  disable_preemption();
  kio_printf("Preemption Started\n");

  kill_cur_thread();
}
