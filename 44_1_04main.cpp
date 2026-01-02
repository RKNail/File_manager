#include "os_file.h"
#include <stdio.h>


int main()
{
    file_manager_t mm;
    setup_file_manager(&mm);
    int TOTAL_SIZE = 500;
    printf("%d, expected 1\n", mm.create(1000));
    printf("%d, expected 0\n", mm.create_file("test_file", 1000+1));
    printf("%d, expected 1\n", mm.destroy());
    printf("%d, expected 0\n", mm.destroy());
    printf("%d, expected 1\n", mm.create(1000));
    printf("%d, expected 0\n", mm.create(1000));
    printf("%d, expected 1\n", mm.destroy());
    // printf("%d, create size 1000, expected 1\n", fm.create(1000));
    // printf("%d, mk ../file1.txt, expected 0\n", fm.create_file("../file1.txt", 1000));
    // printf("%d, mk file2.txt, expected 0\n", fm.create_file("file2.txt", 1));
    // printf("%d, rm file1.txt, expected 1\n", fm.remove("file1.txt", 0));
    // printf("%d, rm file1.txt, expected 0\n", fm.remove("file1.txt", 0));
    // printf("%d, mk file2.txt, expected 1\n", fm.create_file("file2.txt",1000/2));
    // printf("%d, mk file2.txt, expected 0\n", fm.create_file("file2.txt",1000/2));
    // printf("%d, mk file3.txt, expected 1\n", fm.create_file("file3.txt",1000/2));
    // printf("%d, mk test_dir, expected 1\n", fm.create_dir("test_dir"));
    // printf("%d, rm file2.txt, expected 1\n", fm.remove("file2.txt", 0));
    // printf("%d, destroy, expected 1\n", fm.destroy());
     // mm.create(TOTAL_SIZE); //expected: 1, passed
     // mm.create_dir("dir1");// expected: 1, passed
     // mm.create_dir("dir1/dir11"); //expected: 1, passed
     // mm.change_dir("dir1"); //expected: 1, passed
     // mm.create_dir("../dir2"); //expected: 1, passed
     // mm.create_dir("../dir2/dir3"); //expected: 1, passed
     // mm.remove("/dir2/dir3", 0); //expected: 1, passed
     // mm.create_dir("/dir3/dir31"); //expected: 0, passed
     // mm.create_dir("../dir3/dir31"); //expected: 0, passed
     // mm.create_dir("../dir2"); //expected: 0, passed
     // mm.create_file("/dir2/file1", 1); //expected: 1, passed
     // mm.create_dir("/dir2/dir21"); //expected: 1, passed
     // mm.create_dir("/dir2/file1"); //expected: 0, passed
     // mm.create_dir("../dir2/file1"); //expected: 0, passed
     // mm.create_dir("../dir2/file1/dir"); //expected: 0, passed
     // mm.create_dir("../dir2/dir22"); //expected: 1, passed
     // mm.change_dir("dir2"); //expected: 0, passed
     // mm.change_dir("dir11");// expected: 1, passed
     // mm.remove("../../dir2/file1", 0); //expected: 1, passed
     // mm.create_dir("../../dir2/file1");// expected: 1, passed
     // mm.remove("../../dir2/file1", 0); //expected: 1, passed
     // mm.create_file("../../dir2/file1", 1); //expected: 1, passed
     // mm.change_dir(".");// expected: 1, passed
     // mm.change_dir("/dir1/dir11");// expected: 1, passed
     // mm.change_dir("/dir1/dir11/dir3"); //expected: 0, passed
     // mm.change_dir("/dir1");// expected: 1, passed
     // mm.change_dir("./dir11"); //expected: 1, passed
     // mm.change_dir(".."); //expected: 1, passed
     // mm.create_file("./dir11/file.txt", 1); //expected: 1, passed
     // mm.remove("dir11", 0);// expected: 0, passed
     // mm.remove("./dir11", 0);// expected: 0, passed
     // mm.remove("./dir11", 1); //expected: 1, passed
     // mm.list("..",0); //expected: 1, passed
     // mm.list("dir1",0);// expected: 0, passed
     // mm.list(".",0); //expected: 1, passed mm.list("./dir11/file.txt",0); //expected: 0, passed
     // mm.create_file("file11.txt",1); //expected: 1, passed
     // mm.list("./file11.txt",0);// expected: 1, passed
     // mm.list("file11.txt",0);// expected: 1, passed
     // mm.create_file("../dir2/a_file22.txt",1); //expected: 1, passed
     // mm.list("../dir2",0);// expected: 1, passed
     // mm.list("../dir2",1);// expected: 1, passed
     // mm.list("../dir*",1); //expected: 1, passed
     // mm.destroy();// expected: 1, passed
    return 0;
}