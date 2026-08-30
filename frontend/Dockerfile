FROM node:20-slim

WORKDIR /app

COPY package*.json /app/
# Add the flag here to bypass the upstream peer dependency conflict
RUN npm install --legacy-peer-deps

COPY . /app/

EXPOSE 5173

CMD ["npm", "run", "dev", "--", "--host", "0.0.0.0"]
