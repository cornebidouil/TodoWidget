# Documentation Style Guide

This document outlines the documentation standards for the TODO Widget project.

## General Principles

1. **Be Clear and Concise**: Write documentation that is easy to understand but comprehensive.
2. **Be Consistent**: Follow the same style and formatting throughout the codebase.
3. **Document Why, Not Just What**: Explain the rationale behind complex or non-obvious code.
4. **Keep Documentation Updated**: Update documentation when code changes.

## File Documentation

Every source file should begin with a header comment in the following format:

```cpp
/**
 * @file filename.h
 * @brief Brief description of the file's contents
 * 
 * Detailed description of the file's purpose, contents, and any important notes.
 * 
 * @author Your Name
 * @date Last updated: YYYY-MM-DD
 */
```

## Class Documentation

Classes should be documented in the following format:

```cpp
/**
 * @class ClassName
 * @brief Brief description of the class
 * 
 * Detailed description of the class's purpose, functionality, and usage.
 * Include any important implementation details or notes for users of the class.
 */
```

## Method Documentation

Methods should be documented in the following format:

```cpp
/**
 * @brief Brief description of what the method does
 * 
 * Detailed description of the method's functionality, implementation details,
 * and any important notes or caveats.
 * 
 * @param paramName Description of the parameter
 * @param anotherParam Description of another parameter
 * @return Description of what the method returns
 * @throws Description of any exceptions that might be thrown
 */
```

## Property Documentation

Class member variables should be documented with trailing comments in the header file:

```cpp
private:
    int m_propertyName;      ///< Description of the property
    QString m_anotherProperty; ///< Description of another property
```

## Signal Documentation

Signals in Qt classes should be documented like methods, explaining when they are emitted:

```cpp
signals:
    /**
     * @brief Signal emitted when something happens
     * 
     * Detailed description of when the signal is emitted and why.
     * 
     * @param param Description of the parameter
     */
    void signalName(ParameterType param);
```

## Slot Documentation

Slots should be documented like regular methods, with the additional note of which signals they typically connect to:

```cpp
private slots:
    /**
     * @brief Handle some event
     * 
     * Detailed description of what this slot does.
     * Usually connected to SomeClass::someSignal.
     * 
     * @param param Description of the parameter
     */
    void onSomethingHappened(ParameterType param);
```

## Code Examples

When appropriate, include code examples in documentation to demonstrate usage:

```cpp
/**
 * @brief Convert a task to JSON format
 * 
 * Example:
 * @code
 * Task task("Buy milk", "groceries");
 * QJsonObject json = task.toJson();
 * @endcode
 * 
 * @return QJsonObject containing the task data
 */
QJsonObject toJson() const;
```

## Conditional Code Documentation

For conditional or platform-specific code, document the conditions:

```cpp
/**
 * @brief Initialize platform-specific features
 * 
 * On Windows, this sets up taskbar integration.
 * On macOS, this sets up menu bar integration.
 * On Linux, this sets up desktop environment integration.
 */
void initializePlatformSpecific();
```

## TODO Comments

Use TODO comments to mark code that needs future attention:

```cpp
// TODO: Implement proper error handling here
```

## Doxygen Tags Reference

Common Doxygen tags used in this project:

- `@file`: Identifies the file being documented
- `@brief`: Short description
- `@class`: Identifies the class being documented
- `@param`: Documents a parameter
- `@return`: Documents the return value
- `@throws`: Documents exceptions thrown
- `@code`, `@endcode`: Encloses example code
- `@see`: References related entities
- `@todo`: Documents planned work
- `@deprecated`: Marks deprecated features
- `@since`: Indicates when a feature was introduced
- `@note`: Adds a note for special attention

## Generating Documentation

Documentation is generated using Doxygen. To generate documentation:

1. Install Doxygen
2. Run `doxygen Doxyfile` from the project root
3. Documentation will be generated in the `docs/generated` directory

## Best Practices

1. Document all public and protected members
2. Document private members when their purpose is not immediately obvious
3. Use complete sentences with proper punctuation
4. Keep line length reasonable (around 80-100 characters)
5. Update documentation when code changes
6. Focus on why code does something, not just what it does
7. Include performance characteristics for computationally intensive operations
8. Document any assumptions or preconditions
