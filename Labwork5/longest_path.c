#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char key[20];
    int value;
} KeyValue;

KeyValue *intermediate_data = NULL;
int intermediate_count = 0;
int intermediate_capacity = 0;

#define INITIAL_CAPACITY 1000

void ensure_capacity() {
    if (intermediate_count >= intermediate_capacity) {
        intermediate_capacity = (intermediate_capacity == 0) ? INITIAL_CAPACITY : intermediate_capacity * 2;
        intermediate_data = (KeyValue *)realloc(intermediate_data, intermediate_capacity * sizeof(KeyValue));
        if (intermediate_data == NULL) {
            perror("Error reallocating memory");
            exit(EXIT_FAILURE);
        }
    }
}

void mapper(char *line) {
    size_t length = strlen(line);
    
    if (length > 0 && line[length - 1] == '\n') {
        length--;
    }

    if (length > 0) {
        ensure_capacity();
        strcpy(intermediate_data[intermediate_count].key, "max_len_key");
        intermediate_data[intermediate_count].value = (int)length;
        intermediate_count++;
    }
}

int compare_keys(const void *a, const void *b) {
    return strcmp(((KeyValue *)a)->key, ((KeyValue *)b)->key);
}

void reducer() {
    printf("LONGEST PATH LENGTH (OUTPUT OF REDUCER)\n");

    if (intermediate_count == 0) {
        printf("<Longest_Path_Length, 0>\n");
        return;
    }

    qsort(intermediate_data, intermediate_count, sizeof(KeyValue), compare_keys);

    int max_length = 0;
    
    for (int i = 0; i < intermediate_count; i++) {
        int current_length = intermediate_data[i].value;
        if (current_length > max_length) {
            max_length = current_length;
        }
    }

    printf("<Longest_Path_Length, %d>\n", max_length);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uses: %s <tên_file_input>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Cannot open file input");
        return EXIT_FAILURE;
    }

    char line[4096];
    
    while (fgets(line, sizeof(line), file) != NULL) {
        char *line_copy = strdup(line);
        if (line_copy == NULL) {
             perror("Error allocating memory");
             fclose(file);
             free(intermediate_data);
             return EXIT_FAILURE;
        }
        mapper(line_copy);
        free(line_copy);
    }
    
    fclose(file);

    reducer();
    
    free(intermediate_data);
    
    return 0;
}