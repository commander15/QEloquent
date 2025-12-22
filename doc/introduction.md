# Introduction {#intro}

Welcome to the **QEloquent** documentation. QEloquent is a modern, flexible ORM for Qt 6 that brings the power and elegance of Laravel's Eloquent to C++.

## Core Concepts

QEloquent is built around several key pillars that make database management in Qt applications a breeze:

- **@ref model_definition "Models"**: Your database tables represented as C++ classes using standard Qt properties.
- **@ref usage "Simple CRUD"**: Intuitive static and instance methods for Creating, Reading, Updating, and Deleting records.
- **@ref error_handling "Error Handling"**: A robust `Result<T, E>` pattern to handle database operations safely without exceptions.
- **@ref relations "Relationship Handling"**: Define and traverse 1:1 and 1:N relations with ease using a user-friendly, transparently loading API.

## Why QEloquent?

- **Familiarity**: If you know Eloquent (Laravel) or ActiveRecord (Rails), you'll feel right at home.
- **Qt Integration**: It doesn't fight against Qt; it embraces it. Using `Q_GADGET` means zero boilerplate for reflection.
- **Performance**: Lightweight and built on top of the battle-tested `QSqlDatabase` module.
- **Modern C++**: Built with C++20 standards for maximum safety and expressiveness.

Next, let's learn how to [define your first Model](@ref model_definition).
