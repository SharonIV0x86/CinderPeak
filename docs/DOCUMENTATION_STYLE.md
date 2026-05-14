# CinderPeak Documentation Style Guide

This document defines the documentation standards for CinderPeak source code and API references.

## Goals

The documentation system aims to:
- Maintain consistent API documentation
- Improve contributor onboarding
- Support future auto-generated API references
- Document architectural intent and performance characteristics

---

# General Principles

- Document public-facing APIs thoroughly
- Avoid redundant comments that simply restate implementation
- Prefer explaining intent, constraints, and tradeoffs
- Keep documentation colocated with declarations where possible

---

# Doxygen Style

Use block-style Doxygen comments for public APIs.

Example:

```cpp
/**
 * @brief Adds a vertex to the graph.
 *
 * Inserts the provided vertex into the active storage backend.
 *
 * @param vertex Vertex instance to insert.
 *
 * @return true if insertion succeeds.
 * @return false if vertex already exists.
 *
 * @complexity
 * Average: O(1)
 *
 * @threadsafety
 * Concurrent reads are safe.
 */
 ```

---

# Required Tags

## Functions

- `@brief`
- `@param`
- `@return` (if applicable)

## Templates

- `@tparam`

## Exceptions

- `@throws` (if applicable)

---

# Complexity Documentation

Public graph operations should document expected complexity whenever meaningful.

Preferred format:
```cpp
 * @complexity
 * Average: O(1)
 * Worst Case: O(V)
```

---

# Thread Safety Notes

Document thread-safety expectations for public APIs when relevant.

Preferred format:
```cpp
 * @threadsafety
 * Concurrent reads are safe.
```

---

# Examples

Add examples only when they significantly improve API clarity.

Use:
```cpp
 * @code
 * graph.addVertex(vertex);
 * @endcode
 ```

 ---

# Internal Implementation Notes

Avoid excessive inline comments for self-explanatory logic.

Comments should explain:

- architectural intent
- non-obvious decisions
- invariants
- constraints

rather than simple implementation details.

---