#include "matrix.h"

void usage(char *err) {
    char *usage = "\nusage: matrix {routines} [options] -f matrix1 [matrix2]\n"
                "routines:\n"
                "  --sm scalar\tperform scalar multiplcation with value scalar\n"
                "  --tr\t\tcompute the matrix trace value\n"
                "  --ad\t\tperform matrix addition on two matrices, matrix2 must be specified\n"
                "  --ts\t\ttranspose the given matrix\n"
                "  --mm\t\tperform matrix multiplication on two matrices, matrix2 must be specified\n"
                "options:\n"
                "  -t threads\tspecify the number of execution threads to use\n"
                "  -l log\tspecify the log file to output results to\n";
    if (err != NULL) {
        fprintf(stderr, "matrix: %s", err);
    }
    printf("%s", usage);
}

int main(int argc, char *argv[]) {
    // CLA variables
    enum VAR_TYPE type = INVALID;
    struct ROUTINE routine;
    routine.type = UNDEF;
    int rows, cols;
    char *data = NULL;
    char *filename = NULL;
    char *filename2 = NULL;
    bool log = false;
    int arg = 1; // Argument pointer
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    // CLA processing
    if (argc < 4) {
        usage("invalid number of arguments supplied\n");
        exit(EXIT_FAILURE);
    }
    while (arg < argc) {
        if (strcmp(argv[arg], "--sm") == 0) {
            if (routine.type != UNDEF) {
                usage("only one routine may be specified\n");
                exit(EXIT_FAILURE);
            } else if ((arg + 1) == argc) { // Expect scalar value
                usage("no scalar value supplied when using --sm\n");
                exit(EXIT_FAILURE);
            }
            routine.type = SM;
            arg++;
            type = numeric_type(argv[arg]); // Check scalar input type
            if (type == INVALID) {
                usage("scalar value must be numeric\n");
                exit(EXIT_FAILURE);
            } else {
                routine.scalar = strtof(argv[arg], NULL);
                if (errno == ERANGE) {
                    fprintf(stderr, "matrix: failed to convert scalar value '%s' to float\n", argv[arg]);
                    exit(EXIT_FAILURE);
                }
            }
        } else if (strcmp(argv[arg], "--tr") == 0) {
            if (routine.type != UNDEF) {
                usage("only one routine may be specified\n");
                exit(EXIT_FAILURE);
            }
            routine.type = TR;
        } else if (strcmp(argv[arg], "--ad") == 0) {
            if (routine.type != UNDEF) {
                usage("only one routine may be specified\n");
                exit(EXIT_FAILURE);
            }
            routine.type = AD;
        } else if (strcmp(argv[arg], "--ts") == 0) {
            if (routine.type != UNDEF) {
                usage("only one routine may be specified\n");
                exit(EXIT_FAILURE);
            }
            routine.type = TS;
        } else if (strcmp(argv[arg], "--mm") == 0) {
            if (routine.type != UNDEF) {
                usage("only one routine may be specified\n");
                exit(EXIT_FAILURE);
            }
            routine.type = MM;
        } else if (strcmp(argv[arg], "-t") == 0) {
            if ((arg + 1) == argc) { // Expect thread value
                usage("number of execution threads must be specified when using -t\n");
                exit(EXIT_FAILURE);
            }
            arg++;
            type = numeric_type(argv[arg]); // Check thread input type
            if (type != TYPE_INT) {
                usage("invalid number of execution threads\n");
                exit(EXIT_FAILURE);
            }  else {
                continue; // TODO: CONVERT TO INT AND ASSIGN TO OMP NUM OF THREADS
                // TODO: NEED TO CHECK FOR MULTIPLE -T PARAM
            }
        } else if (strcmp(argv[arg], "-l") == 0) {
            if (log) { // Logfile already specified
                usage("log parameter should only be used once\n");
                exit(EXIT_FAILURE);
            } else {
                log = true;
            }
        } else if (strcmp(argv[arg], "-f") == 0) {
            if (filename != NULL) { // File already specified
                usage("file parameter should only be used onece\n");
                exit(EXIT_FAILURE);
            } else if ((arg + 1) == argc) { // Expect matrix file
                usage("at least one matrix file must be specified after -f\n");
                exit(EXIT_FAILURE);
            } else if (strcmp(argv[arg+1], "--sm") == 0 || strcmp(argv[arg+1], "--tr") == 0 || strcmp(argv[arg+1], "--ad") == 0
                    || strcmp(argv[arg+1], "--ts") == 0 || strcmp(argv[arg+1], "--mm") == 0 || strcmp(argv[arg+1], "-t") == 0
                    || strcmp(argv[arg+1], "-l") == 0 || strcmp(argv[arg+1], "-f") == 0) {
                usage("at least one matrix file must be specified after -f\n");
                exit(EXIT_FAILURE);
            } else {
                arg++;
                filename = argv[arg];
                // Check for second matrix file if any
                if ((arg + 1) != argc && strcmp(argv[arg+1], "--sm") != 0 && strcmp(argv[arg+1], "--tr") != 0 && strcmp(argv[arg+1], "--ad") != 0
                    && strcmp(argv[arg+1], "--ts") != 0 && strcmp(argv[arg+1], "--mm") != 0 && strcmp(argv[arg+1], "-t") != 0
                    && strcmp(argv[arg+1], "-l") != 0 && strcmp(argv[arg+1], "-f") != 0) {
                    arg++;
                    filename2 = argv[arg];
                }
            }
        } else {
            fprintf(stderr, "matrix: unknown parameter '%s'\n", argv[arg]);
            usage(NULL);
            exit(EXIT_FAILURE);
        }
        arg++;
    }

    // File validation
    if (filename == NULL) {
        usage("no matrix file specified\n");
        exit(EXIT_FAILURE);
    }
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "%s: no such file\n", filename);
		exit(EXIT_FAILURE);
	}
    if (filename2 != NULL && routine.type != MM) {
        usage("only one matrix input file is required with routine\n");
        exit(EXIT_FAILURE);
    }

    switch (routine.type) {
        case SM:
            // Read input file
            type = read_mat_type(fp);
            rows = read_mat_dim(fp);
            cols = read_mat_dim(fp);
            data = read_line(fp);
            struct COO matrix = coo_format(rows, cols, type, data);

            // Perform the scalar multiplication routine
            scalar_multiply(matrix, routine.scalar);
            matrix.type = TYPE_FLOAT; // Float scalar results in float matrix

            if (log) {
                char *output_file = get_output_name(tm, "sm");
                FILE *output = fopen(output_file, "w"); // sample file
                if(output == NULL) {
                    fprintf(stderr, "matrix: failed to generate output file\n");
                    exit(EXIT_FAILURE);
                }
                write_details(output, filename, filename2, rows, cols, routine.type, matrix.type);
                write_coo_data(output, matrix);
                printf("matrix: successfully generated output to '%s'\n", output_file);
                fclose(output);
                free(output_file);
            } else {
                write_details(stdout, filename, filename2, rows, cols, routine.type, matrix.type);
                write_coo_data(stdout, matrix);
            }
            break;
        case TR:
            break;
        case AD:
            break;
        case TS:
            break;
        case MM:
            break;
        case UNDEF: // No routines specified
            usage("no matrix algebra routine specified\n");
            exit(EXIT_FAILURE);
    }
    fclose(fp);
    free(data);
    exit(EXIT_SUCCESS);
}
