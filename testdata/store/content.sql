-- Insert User roles
INSERT INTO UserRoles (name) VALUES
('Seller');

-- Insert User groups
INSERT INTO UserGroups (name) VALUES
('Stock Managers'),
('Customer Advisors');

-- Insert Users
INSERT INTO Users (name, email, password, role_id) VALUES
-- Pass: loki_lock
('Amadou Benjamain', 'amadoubenjamain@store.com', 'loki_lock', 1),
-- Pass: mad-max
('John Doe', 'johndoe@store.com', 'mad-max', 1);

-- Linking users to groups
INSERT INTO UserGroupMembers (group_id, user_id) VALUES
(1, 1),
(2, 1);

-- Insert Categories
INSERT INTO Categories (name, description) VALUES    
('Fruits', 'Natural meals'),
('Milky', 'Food full of milk');

-- Insert Products
INSERT INTO Products (name, description, price, barcode, category_id) VALUES
('Apple', 'Fresh red apple', 0.50, '1234567890123', 1),
('Banana', 'Organic banana', 0.30, '2234567890123', 1),
('Milk', '1L whole milk', 1.20, '3234567890123', 2);

-- Insert Stocks
INSERT INTO Stocks (quantity, product_id) VALUES
(100, 1),
(150, 2),
(80, 3);

-- Insert Sales
INSERT INTO Sales (number, amount, seller_id) VALUES
(1, 3.00, 1),
(2, 1.80, 2);

-- Insert SaleItems
INSERT INTO SaleItems (unit_price, quantity, sale_id, product_id) VALUES
(0.50, 2, 1, 1),
(1.20, 1, 1, 3),
(0.30, 3, 2, 2);

