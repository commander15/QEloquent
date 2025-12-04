# Store Database

This is a test database, intended for RestLink SQL plugin testing.

---

## Tables

- **DatabaseVersions**: track database evolution using versioning.
- **Users**, **UserRoles**, **UserGroups** and **UserGroupMembers**: user related informations.
- **Products** and **Categories**: respectively products and product categories informations.
- **Sales** and **SaleItems**: respectively sales and sale items informations.

## Relations

Here are some relations to be used for testing.

| Main Table       | Target table | Pivot / Intermediate |  Foreign key     | Local / Owner key | Type                   |
|------------------|--------------|----------------------|------------------|-------------------|------------------------|
| **Users**        | UserRoles    | -                    | role_id          | id                | Belongs To One         |
|                  | UserGroups   | UserGroupMembers     | group_id         | user_id           | Belongs To Many        |
|                  | Sales        | -                    | seller_id        | id                | Belongs To Many        |
| **Products**     | Categories   | -                    | category_id      | id                | Belongs To One         |
|                  | Stocks       | -                    | product_id       | id                | Has One                |
|                  | Sales        | SaleItems            | product_id       | sale_id           | Belongs To Many Though |
|                  | SaleItems    | -                    | product_id       | id                | Has Many               |
| **Categories**   | Products     | -                    | category_id      | id                | Belongs To Many        |
|                  | Sales        | SaleItems + Products | product_id       | sale_id           | Belongs To Many Though |
| **Sales**        | Users        | -                    | seller_id        | id                | Belongs To One         |
|                  | Products     | SaleItems            | product_id       | sale_id           | Has Many               |
|                  | Categories   | SaleItems + Products | category_id      | product_id        | Has Many Though        |

*Incomplete, work in progress.*

## Configuration files

To ease the configuration, we splited relations meta informations accross several files.

| Configuration file        | Description                                                                                |
|---------------------------|--------------------------------------------------------------------------------------------|
| **configuration_0.json**  | Contains custom endpoints that map HTTP requests to raw SQL queries.                       |
| **configuration_1.json**  | Contains authentication config and basic resources informations (no relations data).       |

*Incomplete, work in progress.*

## Scripts

These are SQL scripts for database creation and filling.

| Script file                 | Description                                             |
|-----------------------------|---------------------------------------------------------|
| **structure.sql**           | Contains statements to create database tables.          |
| **content.sql**             | Contatins statements to fill some data on the database. |
