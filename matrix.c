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
                "  -l\t\tif present, results will be logged to an output file\n";
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
    char *data2 = NULL;
    char *filename = NULL;
    char *filename2 = NULL;
    bool log = false;
    int arg = 1; // Argument pointer
    int threads = -1;

    // Timing and outfile name variables
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    float load_time, routine_time;
    struct timeval start, end;

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
                routine.scalar = strtod(argv[arg], NULL);
                if (errno == ERANGE) {
                    fprintf(stderr, "matrix: failed to convert scalar value '%s' to double\n", argv[arg]);
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
            }  else if (threads != -1) {
                usage("thread parameter should only be used once\n");
                exit(EXIT_FAILURE);
            }
            threads = strtoimax(argv[arg], NULL, 10);
            if (errno == EINVAL) {
                fprintf(stderr, "matrix: error processing thread value '%s'\n", argv[arg]);
                exit(EXIT_FAILURE);
            } else if (threads <= 0) {
                usage("number of exeuction threads must be greater than 0\n");
                exit(EXIT_FAILURE);
            } else {
                omp_set_num_threads(threads);
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
    if (filename2 != NULL && routine.type != MM && routine.type != AD) {
        usage("only one matrix input file is required with this routine\n");
        exit(EXIT_FAILURE);
    } else if (filename2 == NULL && (routine.type == MM || routine.type == AD)) {
        usage("two matrix input files are required with this routine\n");
        exit(EXIT_FAILURE);
    }

    switch (routine.type) {
        case SM:
            // Read input file
            gettimeofday(&start, NULL);
            type = read_mat_type(fp);
            rows = read_mat_dim(fp);
            cols = read_mat_dim(fp);
            data = read_line(fp);
            struct COO coo_matrix = coo_format(rows, cols, type, data);
            gettimeofday(&end, NULL);
            load_time = get_time(start, end);

            // Perform the scalar multiplication routine
            gettimeofday(&start, NULL);
            scalar_multiply(coo_matrix, routine.scalar);
            gettimeofday(&end, NULL);
            routine_time = get_time(start, end);
            if (log) {
                char *output_file = get_output_name(tm, "sm");
                FILE *output = fopen(output_file, "w"); // sample file
                if(output == NULL) {
                    fprintf(stderr, "matrix: failed to generate output file\n");
                    exit(EXIT_FAILURE);
                }
                write_details(output, filename, filename2, rows, cols, routine.type, coo_matrix.type);
                coo_matrix.type = TYPE_FLOAT; // Float scalar results in float matrix
                write_coo_data(output, coo_matrix);
                write_times(output, load_time, routine_time);
                printf("matrix: successfully logged results to '%s'\n", output_file);
                fclose(output);
                free(output_file);
            } else {
                write_details(stdout, filename, filename2, rows, cols, routine.type, coo_matrix.type);
                coo_matrix.type = TYPE_FLOAT; // Float scalar results in float matrix
                write_coo_data(stdout, coo_matrix);
                write_times(stdout, load_time, routine_time);
            }
            break;
        case TR:
            // Read input file
            gettimeofday(&start, NULL);
            type = read_mat_type(fp);
            rows = read_mat_dim(fp);
            cols = read_mat_dim(fp);
            if (rows != cols) {
                fprintf(stderr, "matrix: the trace routine can only be performed on square matrices\n");
                exit(EXIT_FAILURE);
            }
            data = read_line(fp);
            struct CSR trmatrix = csr_format(rows, cols, type, data);
            gettimeofday(&end, NULL);
            load_time = get_time(start, end);

            // Perform the trace routine
            union {
                int i;
                double f;
            } trace_result;
            if (trmatrix.type == TYPE_INT) {
                gettimeofday(&start, NULL);
                trace_result.i = trace(trmatrix);
                gettimeofday(&end, NULL);
                routine_time = get_time(start, end);
            } else {
                gettimeofday(&start, NULL);
                trace_result.f = trace_f(trmatrix);
                gettimeofday(&end, NULL);
                routine_time = get_time(start, end);
            }
            
            if (log) {
                char *output_file = get_output_name(tm, "tr");
                FILE *output = fopen(output_file, "w"); // sample file
                if(output == NULL) {
                    fprintf(stderr, "matrix: failed to generate output file\n");
                    exit(EXIT_FAILURE);
                }
                write_details(output, filename, filename2, rows, cols, routine.type, trmatrix.type);

                // Write single trace value
                if (trmatrix.type == TYPE_INT) {
                    fprintf(output, "%d\n", trace_result.i); 
                } else {
                    fprintf(output, "%f\n", trace_result.f);
                }
                write_times(output, load_time, routine_time);
                printf("matrix: successfully logged results to '%s'\n", output_file);
                fclose(output);
                free(output_file);
            } else {
                write_details(stdout, filename, filename2, rows, cols, routine.type, trmatrix.type);
                if (trmatrix.type == TYPE_INT) {
                    fprintf(stdout, "%d\n", trace_result.i); 
                } else {
                    fprintf(stdout, "%f\n", trace_result.f);
                }
                write_times(stdout, load_time, routine_time);
            }
            break;
        case AD:
            // Read input files
            gettimeofday(&start, NULL);
            type = read_mat_type(fp);
            rows = read_mat_dim(fp);
            cols = read_mat_dim(fp);
            data = read_line(fp);
            struct CSR admatrix = csr_format(rows, cols, type, data);
            gettimeofday(&end, NULL);
            load_time = get_time(start, end);

            // Filename2 validation
            FILE *fp2 = fopen(filename2, "r");
            if (fp2 == NULL) {
                fprintf(stderr, "%s: no such file\n", filename2);
                exit(EXIT_FAILURE);
            }

            gettimeofday(&start, NULL);
            type = read_mat_type(fp2);
            rows = read_mat_dim(fp2);
            cols = read_mat_dim(fp2);
            data2 = read_line(fp2);
            struct CSR admatrix2 = csr_format(rows, cols, type, data2);

            gettimeofday(&end, NULL);
            load_time += get_time(start, end);

            // Check matrix constraints
            if (admatrix.rows != admatrix2.rows || admatrix.cols != admatrix2.cols) {
                fprintf(stderr, "matrix: the addition routine can only be performed on matrices with identical dimensions\n");
                exit(EXIT_FAILURE);
            } else if (admatrix.type != admatrix2.type) {
                fprintf(stderr, "matrix: the addition routine should only be performed on matrices of identical variable types\n");
                exit(EXIT_FAILURE);
            }
            struct CSR adresult;

            gettimeofday(&start, NULL);
            if (admatrix.type == TYPE_INT) {
                adresult = matrix_addition(admatrix, admatrix2);
            } else {
                adresult = matrix_addition_f(admatrix, admatrix2);
            }
            gettimeofday(&end, NULL);
            routine_time = get_time(start, end);
            
            if (log) {
                char *output_file = get_output_name(tm, "ad");
                FILE *output = fopen(output_file, "w"); // sample file
                if(output == NULL) {
                    fprintf(stderr, "matrix: failed to generate output file\n");
                    exit(EXIT_FAILURE);
                }
                write_details(output, filename, filename2, rows, cols, routine.type, adresult.type);
                write_csr_data(output, adresult);
                write_times(output, load_time, routine_time);
                printf("matrix: successfully logged results to '%s'\n", output_file);
                fclose(output);
                free(output_file);
            } else {
                write_details(stdout, filename, filename2, rows, cols, routine.type, adresult.type);
                write_csr_data(stdout, adresult);
                write_times(stdout, load_time, routine_time);
            }
            break;
        case TS:
            // Read input file
            gettimeofday(&start, NULL);
            type = read_mat_type(fp);
            rows = read_mat_dim(fp);
            cols = read_mat_dim(fp);
            data = read_line(fp);
            struct CSC tsmatrix = csc_format(rows, cols, type, data);
            gettimeofday(&end, NULL);
            load_time = get_time(start, end);

            // Perform the scalar multiplication routine
            gettimeofday(&start, NULL);
            struct CSR tsresult = transpose(tsmatrix);
            gettimeofday(&end, NULL);
            routine_time = get_time(start, end);

            if (log) {
                char *output_file = get_output_name(tm, "ts");
                FILE *output = fopen(output_file, "w"); // sample file
                if(output == NULL) {
                    fprintf(stderr, "matrix: failed to generate output file\n");
                    exit(EXIT_FAILURE);
                }
                write_details(output, filename, filename2, rows, cols, routine.type, tsresult.type);
                write_csr_data(output, tsresult);
                write_times(output, load_time, routine_time);
                printf("matrix: successfully logged results to '%s'\n", output_file);
                fclose(output);
                free(output_file);
            } else {
                write_details(stdout, filename, filename2, rows, cols, routine.type, tsresult.type);
                write_csr_data(stdout, tsresult);
                write_times(stdout, load_time, routine_time);
            }
            break;
        case MM:
            break;
        case UNDEF: // No routines specified
            usage("no matrix algebra routine specified\n");
            exit(EXIT_FAILURE);
    }
    fclose(fp);
    free(data);
    free(data2);
    exit(EXIT_SUCCESS);
}
