#ifndef __INTERVAL_TREE_H
#define __INTERVAL_TREE_H

#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

template <class T, class K>
class Interval {
public:
	K start;
	K stop;
	T value;
	Interval(K s, K e, const T& v)
		: start(s)
		, stop(e)
		, value(v)
	{ }
};

template <class T, class K>
int intervalStart(const Interval<T,K>& i) {
	return i.start;
}

template <class T, class K>
K intervalStop(const Interval<T,K>& i) {
	return i.stop;
}

template <class T, class K>
ostream& operator<<(ostream& out, Interval<T,K>& i) {
	out << "Interval(" << i.start << ", " << i.stop << "): " << i.value;
	return out;
}

template <class T, class K>
class IntervalStartSorter {
public:
	bool operator() (const Interval<T,K>& a, const Interval<T,K>& b) {
		return a.start < b.start;
	}
};

template <class T, class K>
class IntervalTree {

public:
	vector<Interval<T,K> > intervals;
	IntervalTree<T,K>* left;
	IntervalTree<T,K>* right;
	K center;

	IntervalTree<T,K>(void)
		: left(NULL)
		, right(NULL)
		, center(0)
	{ }

	IntervalTree<T,K>(const IntervalTree<T,K>& other) {
		center = other.center;
		intervals = other.intervals;
		if (other.left) {
			left = (IntervalTree<T,K>*) malloc(sizeof(IntervalTree<T,K>));
			*left = *other.left;
		} else {
			left = NULL;
		}
		if (other.right) {
			right = new IntervalTree<T,K>();
			*right = *other.right;
		} else {
			right = NULL;
		}
	}

	IntervalTree<T,K>& operator=(const IntervalTree<T,K>& other) {
		center = other.center;
		intervals = other.intervals;
		if (other.left) {
			left = new IntervalTree<T,K>();
			*left = *other.left;
		} else {
			left = NULL;
		}
		if (other.right) {
			right = new IntervalTree<T,K>();
			*right = *other.right;
		} else {
			right = NULL;
		}
	return *this;
	}

	IntervalTree<T,K>(
			vector<Interval<T,K> >& ivals,
			unsigned int depth = 16,
			unsigned int minbucket = 64,
			K leftextent = 0,
			K rightextent = 0,
			unsigned int maxbucket = 512
			)
		: left(NULL)
		, right(NULL)
	{

		--depth;
		if (depth == 0 || (ivals.size() < minbucket && ivals.size() < maxbucket)) {
			intervals = ivals;
			IntervalStartSorter<T,K> intervalStartSorter;
			sort(intervals.begin(), intervals.end(), intervalStartSorter);
		} else {
			if (leftextent == 0 && rightextent == 0) {
				// sort intervals by start
				IntervalStartSorter<T,K> intervalStartSorter;
				sort(ivals.begin(), ivals.end(), intervalStartSorter);
			}

			K leftp = 0;
			K rightp = 0;
			K centerp = 0;
			
			if (leftextent || rightextent) {
				leftp = leftextent;
				rightp = rightextent;
			} else {
				leftp = ivals.front().start;
				vector<K> stops;
				stops.resize(ivals.size());
				transform(ivals.begin(), ivals.end(), stops.begin(), intervalStop<T,K>);
				rightp = *max_element(stops.begin(), stops.end());
			}

			centerp = ivals.at(ivals.size() / 2).start;
			center = centerp;

			vector<Interval<T,K> > lefts;
			vector<Interval<T,K> > rights;

			for (typename vector<Interval<T,K> >::iterator i = ivals.begin(); i != ivals.end(); ++i) {
				Interval<T,K>& interval = *i;
				if (interval.stop < center) {
					lefts.push_back(interval);
				} else if (interval.start > center) {
					rights.push_back(interval);
				} else {
					intervals.push_back(interval);
				}
			}

			if (!lefts.empty()) {
				left = new IntervalTree<T,K>(lefts, depth, minbucket, leftp, centerp);
			}
			if (!rights.empty()) {
				right = new IntervalTree<T,K>(rights, depth, minbucket, centerp, rightp);
			}
		}
	}

	void findOverlapping(K start, K stop, vector<Interval<T,K> >& overlapping) {
		if (!intervals.empty() && ! (stop < intervals.front().start)) {
			for (typename vector<Interval<T,K> >::iterator i = intervals.begin(); i != intervals.end(); ++i) {
				Interval<T,K>& interval = *i;
				if (interval.stop >= start && interval.start <= stop) {
					overlapping.push_back(interval);
				}
			}
		}

		if (left && start <= center) {
			left->findOverlapping(start, stop, overlapping);
		}

		if (right && stop >= center) {
			right->findOverlapping(start, stop, overlapping);
		}

	}

	void findContained(K start, K stop, vector<Interval<T,K> >& contained) {
		if (!intervals.empty() && ! (stop < intervals.front().start)) {
			for (typename vector<Interval<T,K> >::iterator i = intervals.begin(); i != intervals.end(); ++i) {
				Interval<T,K>& interval = *i;
				if (interval.start >= start && interval.stop <= stop) {
					contained.push_back(interval);
				}
			}
		}

		if (left && start <= center) {
			left->findContained(start, stop, contained);
		}

		if (right && stop >= center) {
			right->findContained(start, stop, contained);
		}

	}

	~IntervalTree(void) {
		if (left) {
			delete left;
		}
		if (right) {
			delete right;
		}
	}

};

#endif
