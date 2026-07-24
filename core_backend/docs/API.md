# Core Backend API Documentation

## Overview
The Core Backend provides a comprehensive REST API for the Infrastructure Management Platform. This API handles user management, inventory operations, supplier relationships, and business logic.

## Base URL
```
http://localhost:7234/api
```

## Authentication
Currently using basic authentication. Token-based authentication will be implemented in future versions.

## API Endpoints

### Authentication

#### POST /auth/login
Authenticate user and receive session token.

**Request:**
```json
{
  "username": "string",
  "password": "string"
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "token": "string",
    "user": {
      "id": 1,
      "username": "string",
      "email": "string"
    }
  }
}
```

### Users

#### GET /users
Get all users with pagination.

**Query Parameters:**
- `page` (optional): Page number (default: 1)
- `limit` (optional): Items per page (default: 10)

**Response:**
```json
{
  "success": true,
  "data": [
    {
      "id": 1,
      "username": "string",
      "email": "string",
      "is_active": true,
      "created_at": "2024-07-24T00:00:00Z",
      "updated_at": "2024-07-24T00:00:00Z"
    }
  ],
  "pagination": {
    "page": 1,
    "limit": 10,
    "total": 100
  }
}
```

#### GET /users/{id}
Get user by ID.

**Response:**
```json
{
  "success": true,
  "data": {
    "id": 1,
    "username": "string",
    "email": "string",
    "is_active": true,
    "created_at": "2024-07-24T00:00:00Z",
    "updated_at": "2024-07-24T00:00:00Z"
  }
}
```

#### POST /users
Create new user.

**Request:**
```json
{
  "username": "string",
  "email": "string",
  "password": "string"
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "id": 1,
    "username": "string",
    "email": "string",
    "is_active": true,
    "created_at": "2024-07-24T00:00:00Z"
  }
}
```

#### PUT /users/{id}
Update user information.

**Request:**
```json
{
  "email": "string",
  "password": "string"
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "id": 1,
    "username": "string",
    "email": "string",
    "updated_at": "2024-07-24T00:00:00Z"
  }
}
```

#### DELETE /users/{id}
Delete user by ID.

**Response:**
```json
{
  "success": true,
  "message": "User deleted successfully"
}
```

### Products

#### GET /products
Get all products with pagination.

**Query Parameters:**
- `page` (optional): Page number (default: 1)
- `limit` (optional): Items per page (default: 10)
- `category_id` (optional): Filter by category
- `supplier_id` (optional): Filter by supplier

**Response:**
```json
{
  "success": true,
  "data": [
    {
      "id": 1,
      "sku": "string",
      "name": "string",
      "barcode": "string",
      "description": "string",
      "category_id": 1,
      "supplier_id": 1,
      "unit_id": 1,
      "price": 10.99,
      "cost_price": 5.99,
      "is_active": true,
      "created_at": "2024-07-24T00:00:00Z",
      "updated_at": "2024-07-24T00:00:00Z"
    }
  ],
  "pagination": {
    "page": 1,
    "limit": 10,
    "total": 50
  }
}
```

#### POST /products
Create new product.

**Request:**
```json
{
  "sku": "string",
  "name": "string",
  "barcode": "string",
  "description": "string",
  "category_id": 1,
  "supplier_id": 1,
  "unit_id": 1,
  "price": 10.99,
  "cost_price": 5.99
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "id": 1,
    "sku": "string",
    "name": "string",
    "created_at": "2024-07-24T00:00:00Z"
  }
}
```

### Suppliers

#### GET /suppliers
Get all suppliers.

**Response:**
```json
{
  "success": true,
  "data": [
    {
      "id": 1,
      "name": "string",
      "email": "string",
      "phone": "string",
      "address": "string",
      "tax_number": "string",
      "is_active": true,
      "created_at": "2024-07-24T00:00:00Z",
      "updated_at": "2024-07-24T00:00:00Z"
    }
  ]
}
```

#### POST /suppliers
Create new supplier.

**Request:**
```json
{
  "name": "string",
  "email": "string",
  "phone": "string",
  "address": "string",
  "tax_number": "string"
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "id": 1,
    "name": "string",
    "created_at": "2024-07-24T00:00:00Z"
  }
}
```

### Categories

#### GET /categories
Get all categories.

**Response:**
```json
{
  "success": true,
  "data": [
    {
      "id": 1,
      "name": "string",
      "description": "string",
      "is_active": true,
      "created_at": "2024-07-24T00:00:00Z",
      "updated_at": "2024-07-24T00:00:00Z"
    }
  ]
}
```

#### POST /categories
Create new category.

**Request:**
```json
{
  "name": "string",
  "description": "string"
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "id": 1,
    "name": "string",
    "created_at": "2024-07-24T00:00:00Z"
  }
}
```

### Sales

#### GET /sales
Get all sales with pagination.

**Query Parameters:**
- `page` (optional): Page number (default: 1)
- `limit` (optional): Items per page (default: 10)
- `customer_id` (optional): Filter by customer
- `start_date` (optional): Filter by start date
- `end_date` (optional): Filter by end date

**Response:**
```json
{
  "success": true,
  "data": [
    {
      "id": 1,
      "customer_id": 1,
      "total_amount": 100.00,
      "status": "completed",
      "created_at": "2024-07-24T00:00:00Z"
    }
  ],
  "pagination": {
    "page": 1,
    "limit": 10,
    "total": 25
  }
}
```

#### POST /sales
Create new sale.

**Request:**
```json
{
  "customer_id": 1,
  "items": [
    {
      "product_id": 1,
      "quantity": 2,
      "unit_price": 10.99
    }
  ]
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "id": 1,
    "total_amount": 21.98,
    "created_at": "2024-07-24T00:00:00Z"
  }
}
```

### Purchases

#### GET /purchases
Get all purchase orders.

**Response:**
```json
{
  "success": true,
  "data": [
    {
      "id": 1,
      "supplier_id": 1,
      "total_amount": 500.00,
      "status": "pending",
      "created_at": "2024-07-24T00:00:00Z"
    }
  ]
}
```

#### POST /purchases
Create new purchase order.

**Request:**
```json
{
  "supplier_id": 1,
  "items": [
    {
      "product_id": 1,
      "quantity": 10,
      "unit_cost": 5.99
    }
  ]
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "id": 1,
    "total_amount": 59.90,
    "created_at": "2024-07-24T00:00:00Z"
  }
}
```

## Error Responses

All endpoints may return error responses in the following format:

```json
{
  "success": false,
  "error": "Error message description"
}
```

### Common HTTP Status Codes
- `200 OK` - Request successful
- `201 Created` - Resource created successfully
- `400 Bad Request` - Invalid request data
- `401 Unauthorized` - Authentication required
- `403 Forbidden` - Insufficient permissions
- `404 Not Found` - Resource not found
- `500 Internal Server Error` - Server error

## Rate Limiting
Currently not implemented. Will be added in future versions.

## CORS
CORS is enabled for all origins in development mode. Production configuration will restrict origins.