#include <stdio.h>
#include <stdlib.h>
#include <clocale>
#include "os_file.h"
#define MAX_NAME 32
#define MAX_PATH 128

struct node {
    int size;
    char* name;
    node* parent;
    node* next_sibling;
    node* prev_sibling;
    node* children;
};

int free_space = 0;
node* root;
node* cur_node;
node* tmp_node;
bool created = false;

int create(int disk_size)
{
    if (created) {
        return 0;
    }
    char* root_name = (char*)malloc(sizeof(char));
    if (!root_name) {
        return 0;
    }
    root_name[0] = '\0';
    root = (node*)malloc(sizeof(node));
    if (!root) {
        free(root_name);
        return 0;
    }
    created = true;
    free_space = disk_size;
    root->name = root_name;
    root->parent = NULL;
    root->next_sibling = NULL;
    root->prev_sibling = NULL;
    root->children = NULL;
    root->size = 0;
    cur_node = root;
    return 1;
}

// Конкатенирует имена с dst
void pwd_logic(node* obj, char* dst, int lvl) {
    if (obj->parent) { // Поднимаемся до корня, потому что только у него нет родителя
        pwd_logic(obj->parent, dst, lvl+1);
    } else { // Добавляем /, потому что добрались до корня(чтобы начинался со / путь)
        strcat(dst, "/");
        return;
    }
    strcat(dst, obj->name); // Конкатенируем с dst
    if (lvl != 0) { // Чтобы не добавлялся / в конце пути
        strcat(dst, "/");
    }
}

void cur_dir(char* dst) {
    if (!created) {
        return;
    }
    dst[0] = '\0';
    pwd_logic(cur_node, dst, 0);
}

bool validate_path(const char* path) {
    char* cur_obj = (char*)malloc(sizeof(char) * 129);
    if (path[0] != '/') {
        cur_dir(cur_obj);
        strcat(cur_obj, "/");
        strcat(cur_obj, path);
    } else {
        strcpy(cur_obj, path);
    }
    if (strlen(cur_obj) > MAX_PATH) {
        free(cur_obj);
        return false;
    }
    free(cur_obj);
    return true;
}

bool validate_name(const char* name) {
    // Проверка на длину имени
    if (strlen(name) > MAX_NAME) {
        return false;
    }
    // Проверка на пустую строку
    if (name[0] == '\0') {
        return false;
    }
    // Проверка на . и ..
    if (name[0] == '.') {
        if (name[1] == '.') {
            if (strlen(name) > 2) return false;
            return true;
        }
        if (strlen(name) > 1) return false;
        return true;
    }
    // Проверка на пригодные символы
    for (int i = 0; name[i] != '\0'; i++) {
        if (name[i] >= '0' && name[i] <= '9'
            || name[i] == '.'
            || name[i] >= 'a' && name[i] <= 'z'
            || name[i] >= 'A' && name[i] <= 'Z'
            || name[i] == '_') {
            continue;
        }
        return false;
    }
    return true;
}

// Выделяем имена между / и возвращаем строку
char* dir_name(const char* path, int offset, int* len) {
    char* name = (char*)malloc(sizeof(char) * 33);
    if (name) {
        int counter = 0;
        for (int i = offset; path[i] != '\0' && path[i] != '/'; i++) {
            name[counter++] = path[i];
            if (counter == 33) {
                free(name);
                return NULL;
            }
        }
        name[counter] = '\0';
        if (counter < 33 && counter >= 0) {
            char* tmp = (char*)realloc(name, sizeof(char) * (counter + 1));
            if (tmp) {
                name = tmp;
                *len = counter; //Для сдвига по пути, после взятия имени директории
            } else {
                free(name);
                name = NULL;
            }
        }
    }
    return name;
}

// Ищем нужное нам имя среди объектов у tmp_node, . и .. зарезервированы
// Сравниваем имена искомого объекта и дочернего объекта через strcmp
// При получении значения 0 мы находим нужный объект и возвращаем указатель на него
node* find(const char* name) {
    if (strcmp(name, "..") == 0) {
        return tmp_node->parent;
    }
    if (strcmp(name, ".") == 0) {
        return tmp_node;
    }
    node* tmp;
    if (tmp_node->children) {
        tmp = tmp_node->children;
        while (strcmp(tmp->name, name) != 0 && tmp->next_sibling) {
            tmp = tmp->next_sibling;
        }
        if (strcmp(tmp->name, name) == 0) return tmp;
    }
    return  NULL;
}

int move(const char* path) {
    // Проверка на пригодность пути
    // Если абсолютный путь, то просто проверка на длину
    // Если относительный путь, то добавляем путь текущей директории
    // к данной и проверяем валидность
    if (!validate_path(path)) {
        return 0;
    }
    if (path[0] == '/') { // Для абсолютного пути
        tmp_node = root;
    } else { // Для относительного пути
        tmp_node = cur_node;
    }
    int last_slash = strlen(path);;
    for (int i = 0; path[i] != '\0'; ++i) {
        if (path[i] == '/' && path[i+1] == '\0') {
            last_slash = i;
        }
    }
    char* name;
    int shift;
    // Двигаемся по дереву по данному пути
    for (int i = 0; i < last_slash; ++i) {
        if (path[i] == '/') {
            if (path[i+1] == '/') {
                return 0;
            }
            continue;
        }
        name = dir_name(path, i, &shift);
        i += shift-1;
        if (!validate_name(name)) {
            free(name);
            return 0;
        }
        node* next = find(name);
        if (!next) {
            free(name);
            return 0;
        }
        tmp_node = next;
        free(name);
    }
    return 1;
}

// Передвижение по дереву для создания объектов
// До последнего / мы отделяем имя для создания объекта
// и движемся по директориям до предпоследнего объекта, если возможно
// в end кладётся значение индекса после последнего слеша, если такой есть
int make_move(const char* path, int* end) {
    if (!validate_path(path)) return 0;
    int path_len = strlen(path) - 1;
    int i = path[path_len] == '/' ? path_len - 1 : path_len;
    while (i >= 0 && path[i] != '/') {
        --i;
    }
    if (i < 0) {
        *end = 0;
        tmp_node = cur_node;
        return 1;
    }
    char* move_path = (char*)malloc(sizeof(char)*(i+1));
    if (move_path) {
        strncpy(move_path, path, i);
        move_path[i] = '\0';
        *end = i+1;
        int moved = move(move_path);
        free(move_path);
        return moved;
    }
    return 0;
}

// Основная логика создания объекта
    node* make_object(const char* path) {
    if (!created) {
        return NULL;
    }
    if (!validate_path(path)) {
        return NULL;
    }
    int end;
    int path_len = strlen(path);
    if (!make_move(path, &end)) {
        return NULL;
    }
    if (tmp_node->size != 0) return 0;
    char* obj_name = (char*)malloc(sizeof(char) * (path_len - end + 1));
    if (!obj_name) {
        return NULL;
    }
    int len = 0;
    for (int i = end; path[i] != '\0' && path[i] != '/'; i++) {
        obj_name[len++] = path[i];
    }
    obj_name[len] = '\0';
    char* tmp = (char*)realloc(obj_name, sizeof(char) * (len + 1));
    if (!tmp) {
        free(obj_name);
        return NULL;
    }
    obj_name = tmp;
    if (!validate_name(obj_name)) {
        free(obj_name);
        return 0;
    }
    if (strcmp(obj_name, ".") == 0 || strcmp(obj_name, "..")==0) {
        free(obj_name);
        return 0;
    }
    node* new_child = (node*)malloc(sizeof(node));
    if (!new_child) {
        free(obj_name);
        return NULL;
    }
    new_child->name = obj_name;
    new_child->children = NULL;
    new_child->parent = tmp_node;
    new_child->size = 0;
    new_child->next_sibling = NULL;
    new_child->prev_sibling = NULL;
    if (!tmp_node->children) {
        tmp_node->children = new_child;
    } else {
        node* tmp = tmp_node->children;
        if (strcmp(new_child->name, tmp->name) < 0) {
            new_child->next_sibling = tmp_node->children;
            tmp->prev_sibling = new_child;
            tmp_node->children = new_child;
            return new_child;
        } else if (strcmp(new_child->name, tmp->name) == 0) {
            free(new_child->name);
            free(new_child);
            return NULL;
        }
        while (tmp->next_sibling) {
            if (strcmp(tmp->name, obj_name) == 0) {
                free(obj_name);
                free(new_child);
                return NULL;
            }
            if (strcmp(tmp->name, obj_name) == -1
                 && strcmp(tmp->next_sibling->name, obj_name) == 1) {
                tmp->next_sibling->prev_sibling = new_child;
                new_child->next_sibling = tmp->next_sibling;
                new_child->prev_sibling = tmp;
                tmp->next_sibling = new_child;
                return new_child;
            }
            tmp = tmp->next_sibling;
        }
        if (strcmp(tmp->name, obj_name) == 0) {
            free(obj_name);
            free(new_child);
            return NULL;
        } else {
            tmp->next_sibling = new_child;
            new_child->prev_sibling = tmp;
        }
    }
    return new_child;
}

// Декоратор создания объекта для создания директорий
int mkdir(const char* path) {
    node* new_node = make_object(path);
    return new_node ? 1 : 0;
}

// Декоратор создания объекта для создания файлов
int mkfile(const char* path, int file_size) {
    if (file_size <= 0) return 0;
    if (file_size <= free_space) {
        node* new_node = make_object(path);
        if (new_node) {
            new_node->size = file_size;
            free_space -= file_size;
            return 1;
        }
    }
    return 0;
}

// Перемещаем текущую директорию по пути
int cd(const char* path) {
    if (!created) return 0;
    int res = move(path);
    if (res) {
        cur_node = tmp_node;
    }
    return res;
}

// Рекурсивно движемся по уровням дерева и удаляем элементы
// Если элемент имел размер, то мы добавляем его размер к свободному месту
void delete_node(node* current, int lvl) {
    if (!current) return;
    delete_node(current->children, lvl+1);
    if (lvl != 0) delete_node(current->next_sibling, lvl);
    if (current->size) free_space += current->size;
    if (current->prev_sibling) current->prev_sibling->next_sibling = current->next_sibling;
    if (current->next_sibling) current->next_sibling->prev_sibling = current->prev_sibling;
    free(current->name);
    free(current);
}

// Удаляем объект по пути
// Если не пустая директория и флаг рекурсии 0, то возвращаем ошибку
// Если директория пустая, то мы просто её удаляем
int rm(const char* path, int recursive) {
    if (!created) return 0;
    if (!move(path)) return 0;
    if (tmp_node->size == 0) {
        if (!recursive && tmp_node->children) return 0;
        if (tmp_node == cur_node) cur_node = root;
    }
    if (tmp_node->prev_sibling) tmp_node->prev_sibling->next_sibling = tmp_node->next_sibling;
    if (tmp_node->next_sibling) tmp_node->next_sibling->prev_sibling = tmp_node->prev_sibling;
    if (tmp_node->parent && tmp_node->parent->children == tmp_node) tmp_node->parent->children = tmp_node->next_sibling;
    delete_node(tmp_node, 0);
    return 1;
}

int collapse() {
    int res = rm("/", 1);
    if (res) {
        free_space = 0;
        created = false;
        root = NULL;
        cur_node = tmp_node = root;
    }
    return res;
}

bool starts_with(const char* str, const char* prefix) {
    if (strlen(str) < strlen(prefix)) return false;
    for (int i = 0; prefix[i] != '\0'; i++)
        if (prefix[i] != str[i]) return false;
    return true;
}

bool ends_with(const char* str, const char* suffix) {
    int end = strlen(str);
    int len = strlen(suffix);
    if (end < len) return false;
    while (len >= 0) {
        if (str[end] != suffix[len]) return false;
        --len;
        --end;
    }
    return true;
}

int list(node* current, int dir_first, const char* pref, const char* insert, const char* suffix) {
    if (!current) return 0;
    int return_value = 0;
    char* path = (char*)malloc(sizeof(char)*(MAX_PATH+1));
    path[0] = '\0';
    strcat(path, pref);
    strcat(path, insert);
    strcat(path, suffix);
    node* tmp;
    if (current->size == 0) {
        printf("%s:\n", path);
        if (current->children) tmp = current->children;
        else {
            free(path);
            return 1;
        }
    } else {
        printf("%s\n", path);
        free(path);
        return 1;
    }
    free(path);
    if (dir_first) {
        while (tmp) {
            if (tmp->size == 0) {
                printf("%s\n", tmp->name);
                ++return_value;
            }
            tmp = tmp->next_sibling;
        }
        tmp = current->children;
        while (tmp) {
            if (tmp->size != 0) {
                printf("%s\n", tmp->name);
                ++return_value;
            }
            tmp = tmp->next_sibling;
        }
    } else {
        while (tmp) {
            printf("%s\n", tmp->name);
            ++return_value;
            tmp = tmp->next_sibling;
        }
    }
    return return_value == 0 ? 0 : 1;
}

bool is_template(const char* name) {
    for (int i = 0; name[i] != '\0'; ++i)
        if (name[i] == '*') return true;
    return false;
}

char* get_pref(const char* name) {
    char* pref = (char*)malloc(sizeof(char)*(MAX_NAME+1));
    if (!pref) return NULL;
    for (int i = 0; name[i] != '\0'; ++i) {
        pref[i] = name[i];
        if (pref[i] == '*') {
            pref[i] = '\0';
            char* tmp = (char*)realloc(pref, sizeof(char)*(i+1));
            if (!tmp) {
                free(pref);
                return NULL;
            }
            pref = tmp;
            break;
        }
    }
    return pref;
}

char* get_suffix(const char* name) {
    int len = 0;
    bool begin = false;
    char* suf = (char*)malloc(sizeof(char) * (MAX_NAME+1));
    for (int i = 0; name[i] != '\0'; ++i) {
        if (!begin) {
            if (name[i] != '*') continue;
            begin = true;
            continue;
        }
        if (len == 33) {
            free(suf);
            return NULL;
        }
        suf[len++] = name[i];
    }
    suf[len] = '\0';
    char* tmp = (char*)realloc(suf, sizeof(char)*(len+1));
    if (!tmp) {
        free(suf);
        return NULL;
    }
    suf = tmp;
    return suf;
}

int list_logic(node* current, const char* path, const char* pref, int offset, int dir_first) {
    int len = strlen(path);
    char* suffix = (char*)malloc(sizeof(char)*(MAX_PATH+1));
    if (!suffix) return 0;
    suffix[0] = '\0';
    int last = path[len-1] == '/' ? len - 1 : len;
    for (int i = offset; i < last; ++i) {
        if (path[i] == '/') {
            strcat(suffix, "/");
            continue;
        }
        char* name = dir_name(path, i, &len);
        if (!name) {
            free(suffix);
            return 0;
        }
        i += len;
        strcat(suffix, name);
        node* next = find(name);
        free(name);
        if (!next) return 0;
        tmp_node = next;
    }
    int return_val =  list(tmp_node, dir_first, pref, current->name, suffix);
    free(suffix);
    return return_val;
}

int template_list(const char* path, int dir_first) {
    if (!created) return 0;
    if (!validate_path(path)) return 0;
    bool asterisk = is_template(path);
    bool lock = false;
    if (!asterisk) {
        if (!move(path)) return 0;
        int val = list(tmp_node, dir_first, "", path, "");
        return val;
    }
    int len = strlen(path);
    if (path[0] == '/') tmp_node = root;
    else tmp_node = cur_node;
    int bound = path[len-1] == '/' ? len-1:len;
    int return_value = 0;
    char* pref = (char*)malloc(sizeof(char)*(bound+1));
    if (!pref) return 0;
    pref[0] = '\0';
    for (int i = 0; i < bound; ++i) {
        if (path[i] == '/') {
            strcat(pref, "/");
            continue;
        }
        char* name = dir_name(path, i, &len);
        if (!name) {
            free(pref);
            return 0;
        }
        i += len;
        if (!is_template(name)) {
            node* next = find(name);
            if (!next) {
                free(pref);
                free(name);
                return 0;
            }
            strcat(pref, name);
            strcat(pref, "/");
            tmp_node = next;
        }
        else {
            node* tmp = tmp_node->children;
            char* suffix = get_suffix(name);
            if (!suffix) {
                free(name);
                free(pref);
                return 0;
            }
            char* prefix = get_pref(name);
            if (!prefix) {
                free(name);
                free(pref);
                free(suffix);
                return 0;
            }
            free(name);
            while (tmp) {
                if (starts_with(tmp->name, prefix) && ends_with(tmp->name, suffix)) {
                    tmp_node = tmp;
                    int list_return = list_logic(tmp, path, pref, i, dir_first);
                    if (list_return == 0) {
                        return_value = 0;
                        lock = true;
                    } else if (!lock) {
                        return_value += list_return;
                    }
                }
                tmp = tmp->next_sibling;
            }
            free(pref);
            free(suffix);
            free(prefix);
            break;
        }
        free(name);
    }
    return return_value == 0 ? 0 : 1;
}


void setup_file_manager(file_manager_t *fm) {
    fm->create = create;
    fm->destroy = collapse;
    fm->get_cur_dir = cur_dir;
    fm->create_dir = mkdir;
    fm->change_dir = cd;
    fm->create_file = mkfile;
    fm->remove = rm;
    fm->list = template_list;
}

void dfs(const node* current, const int lvl) {
    if (!current) return;
    current == root ?
    printf("lvl: 0 root\n") : printf("lvl: %d %s with size %d\n", lvl, current->name, current->size);
    dfs(current->children, lvl+1);
    dfs(current->next_sibling, lvl);
}

