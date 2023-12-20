#ifndef	SVM_EXTENSIONS_H
#define SVM_EXTENSIONS_H

/* AHP */
typedef	struct	{
	double	class_value;		/* target or class value */
	int	class_index;		/* the index of this class value as is in the class_labels of svm_class_stats_container below */
	int	num;			/* how many of examples for this? */
	int	correctly_classified;	/* how many were classified correct (in a cross-validation test) ? */
	int	*per_class_classifications; /* how many members of this class were allocated to class X, X is an index from 0,
					and the correspondence to real class labels is given in svm_class_stats_container (*class_labels) */
} svm_class_stats;
typedef	struct	{
	svm_class_stats	**stats;
	int		num_different_classes;
	double		cross_validation_accuracy,		/* this is total_num_correct / total_cases */
			mean_cross_validation_accuracy;		/* this is the mean of per-class accuracies */
	int		num_examples;
	double		*class_labels;				/* the class labels, the index of which refers to per_class_classifications above (svm_class_stats) */
} svm_class_stats_container;
/* end AHP */

/* AHP */
void	print_svm_class_stats_container(FILE *fd, svm_class_stats_container *a_container);
void	free_svm_class_stats(svm_class_stats *a_stats);
void	free_svm_class_stats_container(svm_class_stats_container *a_container);
svm_class_stats_container	*get_class_stats_from_labels(double *labels, int num_labels);
svm_class_stats_container	*get_class_stats_from_problem_raw(int num_examples, double *expected_outputs);
svm_class_stats_container	*get_class_stats_from_problem(svm_problem *a_prob);
svm_class_stats	*find_svm_class_stats_given_class_value(svm_class_stats_container *a_container, double a_class_value, int *location);
void	svm_class_stats_calculate(svm_class_stats_container *a_container);
int	svm_class_stats_count_raw(svm_class_stats_container *a_container, int num_examples, double *actual_outputs, double *expected_outputs);
int	svm_class_stats_count(svm_class_stats_container *a_container, svm_problem *a_prob, double *actual_outputs);
#endif
