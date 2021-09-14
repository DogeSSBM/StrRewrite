# StrRewrite

## Description
A string substitution multiway system.

Input sets of rules, and a starting string.
Starting with the first set of rules, any rule
that can be applied will be applied everywhere
in all combinations of orders and positions and
inter-rule orderings.

Whenever a rule is applied a new node is
generated and the resulting string is compared
to a list of all unique nodes. If the string is
in that list the node and string are discarded
and the parent node instead points to the node
in the list. If the string is unique, the node
is kept as a branch and the process repeats.

If the resulting graph reaches a state where
no more rules in the ruleset can be applied,
the next set of rules is used.

This allows multiple phases of transformation
rules.
