#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "new.h"
#include "Set.h"
#include "Object.h"

enum types{none, uinteger8, uinteger16, uinteger32, sinteger8, sinteger16, sinteger32, single_floating, single_double, character};

struct Set { unsigned count; struct Element * head; };
struct Object { enum types type; void * data; };
struct Element { struct Object * object; struct Element * next; };

static const size_t _Set = sizeof(struct Set);
static const size_t _Object = sizeof(struct Object);
static const size_t _Element = sizeof(struct Element);

const void * Set = & _Set;
const void * Object = & _Object;
const void * Element = & _Element;

void * new (const void * type, ...)
{
    const size_t size = * (const size_t *) type;
	void * p = calloc(1, size);

	assert(p);
	return p;
}

void delete (void * item)
{
	free(item);
}

void * add (void * _set, const void * _object)
{
    struct Set * set = _set;
	struct Object * object = (void *) _object;

	assert(set);
	assert(object);

    if(set->count > 0) // we already have at least one element
    {
        // grab the head node
        struct Element * tmp_elem = set->head;
        for(int cur_elem = 1; cur_elem < set->count; cur_elem++)
        {
            // make sure there is a next
            // if there isn't, it is an error because we should know the count
            assert(tmp_elem->next);
            tmp_elem = tmp_elem->next;
        }
        // replace the end node (which should be null)
        assert(tmp_elem->next == 0);
        tmp_elem->next = new(Element);

        // verify replacement
        assert(tmp_elem->next);

        // advance to the new element
        tmp_elem = tmp_elem->next;

        // add object to next element
        tmp_elem->object = object;

        // increment set count
        set->count++;
    }
    else // the head node should be null since the count is 0
    {
        // create and verify the head element
        set->head = new(Element);
        assert(set->head);

        // point to the object
        set->head->object = object;
        set->count++;
    }
    return object;
}

void * find (const void * _set, const void * _object)
{
    const struct Object * object = _object;
    const struct Set * set = _set;

	assert(set);
	assert(object);

    // look for first reference to object
    struct Element * tmp_elem = set->head;
    while ((tmp_elem != 0) && (tmp_elem->object != object))
    {
        tmp_elem = tmp_elem->next;
    }

    // if we got to the end we didn't find it
    if(tmp_elem == 0)
    {
        return 0;
    }
    else
    {
        assert(tmp_elem->object);
        return (void *) tmp_elem->object;
    }

}

void * find_element (const void * _set, const void * _object)
{
    const struct Object * object = _object;
    const struct Set * set = _set;

	assert(set);
	assert(object);

    // look for first reference to object
    struct Element * tmp_elem = set->head;
    while ((tmp_elem != 0) && (tmp_elem->object != object))
    {
        tmp_elem = tmp_elem->next;
    }

    // if we got to the end we didn't find it
    if(tmp_elem == 0)
    {
        return 0;
    }
    else
    {
        assert(tmp_elem->object == object);
        return (void *) tmp_elem;
    }

}

void * find_prior_element (const void * _set, const void * _object)
{
    const struct Object * object = _object;
    const struct Set * set = _set;

	assert(set);
	assert(object);

    // look for first reference to object
    struct Element * tmp_elem_last = 0;
    struct Element * tmp_elem = set->head;
    while ((tmp_elem != 0) && (tmp_elem->object != object))
    {
        tmp_elem_last = tmp_elem;
        tmp_elem = tmp_elem->next;
    }

    // if we got to the end we didn't find it
    if(tmp_elem == 0)
    {
        return 0;
    }
    else
    {
        return (void *) tmp_elem_last;
    }

}

int contains (const void * _set, const void * _object)
{
	return find(_set, _object) != 0;
}

void * drop (void * _set, const void * _object)
{
    struct Set * set = _set;
	struct Object * object = find(set, _object);

	if (object)
	{
        // we need to find the object just before that object...
        struct Element * prior_element = find_prior_element(set, _object);
        struct Element * element = find_element(set, object);

        // take out element
        if(prior_element) // is there a prior element?
        {
            prior_element->next = element->next;
            delete(element);
        }
        else // if there isn't the head pointer needs to be moved
        {
            set->head = element->next;
            delete(element);
        }

        // decrement set count
        set->count--;
	}
	return object;
}

unsigned count (const void * _set)
{
    const struct Set * set = _set;

	assert(set);
	return set -> count;
}

int differ (const void * a, const void * b)
{
	return a != b;
}
